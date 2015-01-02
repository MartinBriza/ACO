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

#ifndef ACO_H
#define ACO_H

#include <QDebug>
#include <QObject>
#include <QQmlListProperty>
#include <QUrl>
#include <QFile>

#include <random>

class Aco;
class Town;
class Path;
class Ant;
class Canvas;
class Algorithm;

class Ant : public QObject {
    Q_OBJECT
    Q_PROPERTY(Town* town READ town WRITE setTown NOTIFY townChanged)
    Q_PROPERTY(int pos_x READ x NOTIFY xChanged)
    Q_PROPERTY(int pos_y READ y NOTIFY yChanged)
    Q_PROPERTY(QQmlListProperty<Town> taboo READ tabooListProperty NOTIFY tabooChanged)
    Q_PROPERTY(QQmlListProperty<Path> trip READ tripListProperty NOTIFY tripChanged)
public:
    Ant(Algorithm *parent, Town *town);

    Aco *aco();
    Algorithm *algorithm();

    Q_INVOKABLE void step();
    Q_INVOKABLE void reset(Town *t);
    Q_INVOKABLE qreal tripLength();

    Town *town();
    Town *firstTown();
    int x();
    int y();
    QQmlListProperty<Town> tabooListProperty();
    QQmlListProperty<Path> tripListProperty();
    QList<Path*> trip();
public slots:
    void setTown(Town *town);
signals:
    void townChanged();
    void xChanged();
    void yChanged();
    void tabooChanged();
    void tripChanged();
private:
    Town *m_town { nullptr };
    QList<Town*> m_taboo { };
};

class Town : public QObject {
    Q_OBJECT
    Q_PROPERTY(int pos_x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(int pos_y READ y WRITE setY NOTIFY yChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
public:
    Town(Canvas *parent, int x = -1, int y = -1);

    Aco *aco();
    Canvas *canvas();

    int x();
    int y();
    QString name();
public slots:
    void setX(int x);
    void setY(int y);
    void setName(const QString &name);
signals:
    void xChanged();
    void yChanged();
    void nameChanged();
private:
    int m_x { -1 };
    int m_y { -1 };
    QString m_name { };
};

class Path : public QObject {
    Q_OBJECT
    Q_PROPERTY(Town* a READ townA NOTIFY townAChanged)
    Q_PROPERTY(Town* b READ townB NOTIFY townBChanged)
    // simulation properties
    Q_PROPERTY(qreal distance READ distance WRITE setDistance NOTIFY distanceChanged)
    Q_PROPERTY(qreal trail READ trail WRITE setTrail NOTIFY trailChanged)
public:
    Path(Canvas *parent, Town *townA, Town *townB);

    Aco *aco();
    Canvas *canvas();
    Town *townA();
    Town *townB();
    qreal distance();
    qreal trail();
public slots:
    void setDistance(qreal distance);
    void setTrail(qreal trail);
signals:
    void townAChanged();
    void townBChanged();
    void distanceChanged();
    void trailChanged();
private:
    Town *m_a { nullptr };
    Town *m_b { nullptr };
    qreal m_distance { -1.0 };
    qreal m_trail { 1.0 };
};

class Canvas : public QObject {
    Q_OBJECT
    Q_PROPERTY(int townSize READ townSize WRITE setTownSize NOTIFY townSizeChanged)
    Q_PROPERTY(QQmlListProperty<Town> towns READ townsListProperty NOTIFY townsChanged)
    Q_PROPERTY(QQmlListProperty<Path> paths READ pathsListProperty NOTIFY pathsChanged)
    Q_PROPERTY(bool fillPaths READ fillPaths WRITE setFillPaths NOTIFY fillPathsChanged)
    Q_PROPERTY(qreal initialTau READ initialTau WRITE setInitialTau NOTIFY initialTauChanged)
    Q_PROPERTY(qreal animationSpeed READ animationSpeed WRITE setAnimationSpeed NOTIFY animationSpeedChanged)
public:
    Canvas(QObject *parent);
    Q_INVOKABLE Town *townAt(int x, int y);
    Q_INVOKABLE Path *pathBetween(Town *a, Town *b);

    int townSize();
    QList<Town*> &towns();
    QQmlListProperty<Town> townsListProperty();
    QList<Path*> &paths();
    QQmlListProperty<Path> pathsListProperty();
    qreal initialTau();
    qreal animationSpeed();
    bool fillPaths();

public slots:
    void newTown(int x, int y);
    void deleteTown(Town *t);
    void togglePath(Town *a, Town *b);
    bool addPath(Town *a, Town *b);
    bool removePath(Town *a, Town *b);
    void clear();

    void setTownSize(int size);
    void setInitialTau(qreal tau);
    void setFillPaths(bool on);
    qreal setAnimationSpeed(qreal newSpeed);
signals:
    void topologyChanged();
    void distancesChanged();

    void townsChanged();
    void pathsChanged();
    void townSizeChanged();
    void initialTauChanged();
    void fillPathsChanged();
    void animationSpeedChanged();
protected:
    int m_townSize { 40 };
    QList<Town*> m_towns { };
    QList<Path*> m_paths { };
    qreal m_initialTau { 1 };
    bool m_fillPaths { true };
    qreal m_animationSpeed { 100.0 };
};

class Algorithm : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool initialized READ initialized WRITE setInitialized NOTIFY initializedChanged)
    Q_PROPERTY(int antCount READ antCount WRITE setAntCount NOTIFY antCountChanged)
    Q_PROPERTY(QQmlListProperty<Ant> ants READ antsListProperty NOTIFY antsChanged)
    Q_PROPERTY(int c READ c NOTIFY cChanged)
    Q_PROPERTY(int s READ s NOTIFY sChanged)
    Q_PROPERTY(int t READ t NOTIFY tChanged)
    Q_PROPERTY(qreal alpha READ alpha WRITE setAlpha NOTIFY alphaChanged)
    Q_PROPERTY(qreal beta READ beta WRITE setBeta NOTIFY betaChanged)
    Q_PROPERTY(qreal q READ q WRITE setQ NOTIFY qChanged)
    Q_PROPERTY(qreal ro READ ro WRITE setRo NOTIFY roChanged)
    Q_PROPERTY(qreal e READ e WRITE setE NOTIFY eChanged)
    Q_PROPERTY(QQmlListProperty<Path> shortestTrip READ shortestTripProperty NOTIFY shortestTripChanged)
public:
    Algorithm(Aco *parent);
    QList<Ant*> &ants();
    QQmlListProperty<Ant> antsListProperty();

    Aco *aco();

    bool initialized();
    int c();
    int s();
    int t();
    int antCount();
    qreal alpha();
    qreal beta();
    qreal q();
    qreal ro();
    qreal e();
    QQmlListProperty<Path> shortestTripProperty();
public slots:
    void reset();
    void roundInit();
    void step();

    void newAnt(Town *t);
    void setInitialized(bool i);
    void setAntCount(int c);
    void setAlpha(qreal alpha);
    void setBeta(qreal beta);
    void setQ(qreal q);
    void setRo(qreal ro);
    void setE(qreal e);
private slots:
    void slotDistancesChanged();
signals:
    void antsChanged();
    void initializedChanged();
    void cChanged();
    void sChanged();
    void tChanged();
    void antCountChanged();
    void alphaChanged();
    void betaChanged();
    void qChanged();
    void roChanged();
    void eChanged();
    void shortestTripChanged();
protected:
    int m_c { 0 };
    int m_s { 0 };
    int m_t { 0 };
    int m_antCount { 5 };
    qreal m_alpha { 1.0 };
    qreal m_beta { 2.0 };
    qreal m_q { 20.0 };
    qreal m_ro { 0.1 };
    qreal m_e { 2 };

    QList<Ant*> m_ants { };
    QList<Path*> m_shortestTrip { };
    qreal m_shortestTripLength { HUGE_VAL };

    bool m_initialized { false };
};

class AntCycle : public Algorithm {
    Q_OBJECT
public:

};

class Aco : public Canvas {
    Q_OBJECT
    Q_PROPERTY(Canvas *canvas READ canvas NOTIFY canvasChanged)
    Q_PROPERTY(Algorithm *algorithm READ algorithm NOTIFY algorithmChanged)
    Q_PROPERTY(int chosenAlgo READ chosenAlgo WRITE setChosenAlgo NOTIFY chosenAlgoChanged)
public:
    enum Algorithms {
        AntCycle = 0,
        AntDensity,
        AntQuantity,
        ElitistStrategy,
    };
    Q_ENUMS(Algorithm)

    Aco(QObject *parent = 0);
    Q_INVOKABLE QString string();
    Q_INVOKABLE void saveTo(const QUrl &file);
    Q_INVOKABLE void loadFrom(const QUrl &file);
    Q_INVOKABLE qreal getRand();

    Algorithm *algorithm();
    Canvas *canvas();
    int chosenAlgo();

public slots:
    void setChosenAlgo(int a);
private slots:
signals:
    void algorithmChanged();
    void canvasChanged();
    void chosenAlgoChanged();
private:
    std::mt19937 m_mersenneTwister { (long unsigned int) time(0) };
    std::uniform_real_distribution<qreal> m_uniformDist { 0.0, 1.0 };
    Algorithm *m_currentAlgorithm { nullptr };
    Algorithms m_chosenAlgo { AntCycle };
};

#endif // ACO_H
