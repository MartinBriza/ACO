/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Martin Bříza
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include "aco.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qmlRegisterType<Aco>("fit.sfc.aco", 1, 0, "Aco");
    qmlRegisterUncreatableType<Town>("fit.sfc.aco", 1, 0, "Town", "Use the defined API to create towns");
    qmlRegisterUncreatableType<Path>("fit.sfc.aco", 1, 0, "Path", "Use the defined API to create paths");
    qmlRegisterUncreatableType<Ant>("fit.sfc.aco", 1, 0, "Ant", "Use the defined API to create ants");
    qmlRegisterUncreatableType<Canvas>("fit.sfc.aco", 1, 0, "Canvas", "Use the defined API to create a canvas");
    qmlRegisterUncreatableType<Algorithm>("fit.sfc.aco", 1, 0, "Algorithm", "Use the defined API to create algorithms");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
