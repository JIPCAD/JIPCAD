#ifndef DISABLE_MAIN_FOR_TESTS

#include "QtFrontend/MainWindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <chrono>
#include <iostream>
#include <iomanip>
#undef main
int main(int argc, char** argv)
{
    // QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    // QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    //  Setup Qt and set up rendering API
    QApplication application(argc, argv);
    QCoreApplication::setApplicationName("Nome");
    QGuiApplication::setApplicationDisplayName("Nome 3.5");
    QCoreApplication::setOrganizationName("UCBerkeley");
    QCoreApplication::setApplicationVersion("3.5");

    QCommandLineParser parser;
    QCommandLineOption detached3dview { QStringList() << "d"
                                                      << "detached3dview",
                                        "Detach the 3D view from the main window" };
    QCommandLineOption benchmarkMode { QStringList() << "b"
                                                       << "benchmark",
                                         "Run in profiling mode. Modes: 'load', 'subdivide' (or "
                                         "'sd'), 'offset'. Defaults to 'load'.",
                                         "mode", "load" };
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "The file(s) to open.");
    parser.addOption(detached3dview);
    parser.addOption(benchmarkMode);
    parser.process(application);

    bool detach3DView = !parser.isSet(detached3dview);
    bool isBenchmark = parser.isSet(benchmarkMode);
    QString benchMode = parser.value(benchmarkMode).toLower();
    const QStringList posArgs = parser.positionalArguments();

    if (isBenchmark && !posArgs.isEmpty())
    {
        std::cout << "\n==================================================" << std::endl;
        std::cout << " [BENCHMARK RUNNER] Mode: " << benchMode.toStdString() << std::endl;
        std::cout << " File: " << posArgs.first().toStdString() << std::endl;
        std::cout << "==================================================" << std::endl;

        auto totalStart = std::chrono::high_resolution_clock::now();

        // -------------------------------------------------------------
        // PHASE 1: Load Scene Graph & Parse AST
        // -------------------------------------------------------------
        auto loadStart = std::chrono::high_resolution_clock::now();

        Nome::CMainWindow* benchmarkWin = new Nome::CMainWindow(posArgs.first(), nullptr, false);
        benchmarkWin->SetBenchmarkMode(true);
        QCoreApplication::processEvents();

        auto loadEnd = std::chrono::high_resolution_clock::now();
        double loadTimeMs = std::chrono::duration<double, std::milli>(loadEnd - loadStart).count();

        std::cout << std::fixed << std::setprecision(3);
        std::cout << "  [METRIC] Load / AST Parsing Time : " << loadTimeMs << " ms" << std::endl;

        // -------------------------------------------------------------
        // PHASE 2 & 3: Merge and Action Pipeline Execution
        // -------------------------------------------------------------
        if (benchMode == "subdivide" || benchMode == "sd" || benchMode == "offset")
        {
            // 2. Merge Pass (Required prerequisite for subdivision/offset on flattened graph)
            auto mergeStart = std::chrono::high_resolution_clock::now();

            benchmarkWin->on_actionMerge_triggered();
            QCoreApplication::processEvents();

            auto mergeEnd = std::chrono::high_resolution_clock::now();
            double mergeTimeMs =
                std::chrono::duration<double, std::milli>(mergeEnd - mergeStart).count();

            std::cout << "  [METRIC] Merge Pass Time         : " << mergeTimeMs << " ms"
                      << std::endl;

            // 3. Mode-Specific Target Action Pass
            if (benchMode == "subdivide" || benchMode == "sd")
            {
                auto sdStart = std::chrono::high_resolution_clock::now();

                benchmarkWin->on_actionSubdivide_triggered();
                QCoreApplication::processEvents();

                auto sdEnd = std::chrono::high_resolution_clock::now();
                double sdTimeMs =
                    std::chrono::duration<double, std::milli>(sdEnd - sdStart).count();

                std::cout << "  [METRIC] Subdivision Pass Time   : " << sdTimeMs << " ms"
                          << std::endl;
            }
            else if (benchMode == "offset")
            {
                auto offsetStart = std::chrono::high_resolution_clock::now();

                benchmarkWin->on_actionOffset_triggered();
                QCoreApplication::processEvents();

                auto offsetEnd = std::chrono::high_resolution_clock::now();
                double offsetTimeMs =
                    std::chrono::duration<double, std::milli>(offsetEnd - offsetStart).count();

                std::cout << "  [METRIC] Offset Pass Time        : " << offsetTimeMs << " ms"
                          << std::endl;
            }
        }

        auto totalEnd = std::chrono::high_resolution_clock::now();
        double totalTimeMs =
            std::chrono::duration<double, std::milli>(totalEnd - totalStart).count();

        std::cout << "--------------------------------------------------" << std::endl;
        std::cout << "  [METRIC] TOTAL EXECUTION TIME    : " << totalTimeMs << " ms" << std::endl;
        std::cout << "==================================================\n" << std::endl;

        delete benchmarkWin;
        return 0;
    }

    Nome::CMainWindow* mainWindow = nullptr;
    // const QStringList posArgs = parser.positionalArguments();
    for (const QString& file : posArgs)
    {
        Nome::CMainWindow* newWin = new Nome::CMainWindow(file, nullptr, detach3DView);
        newWin->show();
        mainWindow = newWin;
    }

    if (!mainWindow)
        mainWindow = new Nome::CMainWindow(nullptr, detach3DView);

    mainWindow->show();
    application.installEventFilter(mainWindow);

    return application.exec();
}

#endif
