#include <QApplication>
#include <QTimer>
#include <cstdlib>
#include <cmath>
#include "oscilloscopeform.h"

// Глобальный указатель на осциллограф
OscilloscopeForm* g_oscilloscope = nullptr;

extern "C" {
void CreateOscilloscope(int DPos, int DNeg, int AcpShift, int Left, int Top, int Width, int Height);
void SendData(int NumberOfPoints, double* Bufer, int Step);
void GetChannelOptions(int& DPos, int& DNeg, int& AcpShift);
void SetOptions(int DPos, int DNeg, int AcpShift);
}

void CreateOscilloscope(int DPos, int DNeg, int AcpShift, int Left, int Top, int Width, int Height)
{
    if (g_oscilloscope) {
        delete g_oscilloscope;
        g_oscilloscope = nullptr;
    }
    g_oscilloscope = new OscilloscopeForm(nullptr, DPos, DNeg, AcpShift);
    g_oscilloscope->setGeometry(Left, Top, Width, Height);
    g_oscilloscope->show();
}

void SendData(int NumberOfPoints, double* Bufer, int Step)
{
    if (!g_oscilloscope) return;
    QVector<double> data(Bufer, Bufer + NumberOfPoints);
    g_oscilloscope->sendData(data, Step);
}

void GetChannelOptions(int& DPos, int& DNeg, int& AcpShift)
{
    if (g_oscilloscope)
        g_oscilloscope->getOptions(DPos, DNeg, AcpShift);
}

void SetOptions(int DPos, int DNeg, int AcpShift)
{
    if (g_oscilloscope)
        g_oscilloscope->setOptions(DPos, DNeg, AcpShift);
}


static QVector<double> dataBuffer;      // буфер для всех точек (старые не удаляются)
static const int MAX_POINTS = 2000;     // максимальное количество точек
static const int BATCH_SIZE = 100;      // сколько новых точек генерировать за раз
static int phase = 0;                   // текущая фаза для генерации синусоиды
static const int STEP_MS = 10;          // шаг времени между соседними точками (мс)
static QTimer* generationTimer = nullptr; // таймер для периодической генерации

// Генерация одного нового значения (синусоида с шумом)
double generateOnePoint()
{
    double value = 20.0 * sin(2 * M_PI * phase / 50.0) + (rand() % 10 - 5);
    phase++;
    return value;
}

// Генерация пачки новых точек и добавление их в буфер
void generateBatchAndSend()
{
    // Если уже достигли максимума - останавливаем таймер и выходим
    if (dataBuffer.size() >= MAX_POINTS) {
        if (generationTimer) generationTimer->stop();
        return;
    }

    // Сколько точек можно добавить, чтобы не превысить MAX_POINTS
    int remaining = MAX_POINTS - dataBuffer.size();
    int pointsToAdd = (BATCH_SIZE < remaining) ? BATCH_SIZE : remaining;

    // Генерируем и добавляем в буфер
    for (int i = 0; i < pointsToAdd; ++i) {
        dataBuffer.append(generateOnePoint());
    }

    // Отправляем весь буфер (все накопленные точки) в осциллограф
    SendData(dataBuffer.size(), dataBuffer.data(), STEP_MS);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // окно осциллографа
    CreateOscilloscope(10, -10, 0, 100, 100, 800, 600);

    // Таймер
    generationTimer = new QTimer(&a);
    QObject::connect(generationTimer, &QTimer::timeout, []() {
        generateBatchAndSend();
    });
    generationTimer->start(2000);

    generateBatchAndSend();

    return a.exec();
}