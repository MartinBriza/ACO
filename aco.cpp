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

#include "aco.h"
#include <QFile>

////////////////
//                  TOWN
//

Town::Town(Canvas *parent, int x, int y)
    : QObject(parent), m_x(x), m_y(y) {
    static int order = 1;
    m_name = QString("Town %1").arg(order);
    order++;
}

Aco *Town::aco() {
    return qobject_cast<Aco*>(parent());
}

Canvas *Town::canvas() {
    return qobject_cast<Canvas*>(parent());
}

int Town::x() {
    return m_x;
}

int Town::y() {
    return m_y;
}

QString Town::name() {
    return m_name;
}

void Town::setX(int x) {
    if (m_x != x) {
        m_x = x < 0 ? 0 : x;
        emit xChanged();
    }
}

void Town::setY(int y) {
    if (m_y != y) {
        m_y = y < 0 ? 0 : y;
        emit yChanged();
    }
}

void Town::setName(const QString &name) {
    if (m_name != name) {
        m_name = name;
        emit nameChanged();
    }
}



////////////////
//                  PATH
//

Path::Path(Canvas *parent, Town *townA, Town *townB)
    : QObject(parent), m_a(townA), m_b(townB), m_trail(qobject_cast<Canvas*>(parent)->initialTau()) {
    connect(townA, &Town::xChanged, this, &Path::distanceChanged);
    connect(townB, &Town::xChanged, this, &Path::distanceChanged);
    connect(townA, &Town::yChanged, this, &Path::distanceChanged);
    connect(townB, &Town::yChanged, this, &Path::distanceChanged);
}

Aco *Path::aco() {
    return qobject_cast<Aco*>(parent());
}

Canvas *Path::canvas() {
    return qobject_cast<Canvas*>(parent());
}

Town *Path::townA() {
    return m_a;
}

Town *Path::townB() {
    return m_b;
}

qreal Path::distance() {
    if (m_distance < 0.0)
        return sqrt(pow(m_a->x() - m_b->x(), 2) + pow(m_a->y() - m_b->y(), 2)) / 64.0;
    else
        return m_distance;
}

qreal Path::trail() {
    return m_trail;
}

void Path::setDistance(qreal distance) {
    if (m_distance != distance) {
        m_distance = distance;
        emit distanceChanged();
    }
}

void Path::setTrail(qreal trail) {
    if (m_trail != trail) {
        m_trail = trail < canvas()->initialTau() ? canvas()->initialTau() : trail;
        emit trailChanged();
    }
}


////////////////
//                  ANT
//

Ant::Ant(Algorithm *parent, Town *town)
    : QObject(parent), m_town(town) {
    connect(town, &Town::xChanged, this, &Ant::xChanged);
    connect(town, &Town::yChanged, this, &Ant::yChanged);
    connect(this, &Ant::townChanged, this, &Ant::tripChanged);
    m_taboo.append(town);
}

Aco *Ant::aco() {
    return algorithm()->aco();
}

Algorithm *Ant::algorithm() {
    return qobject_cast<Algorithm*>(parent());
}

void Ant::step() {
    qreal totalWeight = 0.0;
    QMap<Town*, qreal> weights;
    qreal target = aco()->getRand();
    qreal current = 0.0;
    for (int i = 0; i < aco()->towns().size(); i++) {
        Town *t = aco()->towns()[i];
        if (!m_taboo.contains(t)) {
            qreal currentWeight = 0.0;
            Path *p = aco()->pathBetween(t, m_taboo.last());
            currentWeight = pow(p->trail(), algorithm()->alpha()) * pow(1.0 / p->distance(), algorithm()->beta());
            totalWeight += currentWeight;
            weights[t] = currentWeight;
        }
    }
    for (Town *t : weights.keys()) {
        current += weights[t] / totalWeight;
        if (current >= target) {
            setTown(t);

            break;
        }
    }
}

void Ant::reset(Town *t) {
    m_taboo.clear();
    setTown(t);
}

qreal Ant::tripLength() {
    qreal ret = 0.0;
    for (int i = 0; i < m_taboo.size() - 1; i++) {
        ret += aco()->pathBetween(m_taboo[i], m_taboo[i + 1])->distance();
    }
    return ret;
}

Town *Ant::town() {
    return m_town;
}

Town *Ant::firstTown() {
    return m_taboo.first();
}

int Ant::x() {
    return m_town->x();
}

int Ant::y() {
    return m_town->y();
}

QQmlListProperty<Town> Ant::tabooListProperty() {
    return QQmlListProperty<Town>(this, m_taboo);
}

QList<Path *> Ant::trip() {
    QList<Path*> paths;
    for (int i = 0; i < m_taboo.size() - 1; i++)
        paths.append(aco()->pathBetween(m_taboo[i], m_taboo[i+1]));
    return paths;
}

QQmlListProperty<Path> Ant::tripListProperty() {
    QList<Path*> tmp = trip();
    return QQmlListProperty<Path>(this, tmp);
}

void Ant::setTown(Town *town) {
    if (m_town != town) {
        disconnect(m_town, &Town::xChanged, this, &Ant::xChanged);
        disconnect(m_town, &Town::yChanged, this, &Ant::yChanged);
        m_town = town;
        m_taboo.append(town);
        connect(town, &Town::xChanged, this, &Ant::xChanged);
        connect(town, &Town::yChanged, this, &Ant::yChanged);
        emit townChanged();
        emit tabooChanged();
        emit xChanged();
        emit yChanged();
    }
}


////////////////
//                  ALGORITHM
//

Algorithm::Algorithm(Aco *parent)
    : QObject(parent) {
    connect(parent, &Canvas::topologyChanged, this, &Algorithm::reset);
    connect(parent, &Canvas::distancesChanged, this, &Algorithm::slotDistancesChanged);
}

QList<Ant *> &Algorithm::ants() {
    return m_ants;
}

QQmlListProperty<Ant> Algorithm::antsListProperty() {
    return QQmlListProperty<Ant>(this, m_ants);
}

Aco *Algorithm::aco() {
    return qobject_cast<Aco*>(parent());
}

bool Algorithm::initialized() {
    return m_initialized;
}


int Algorithm::c() {
    return m_c;
}

int Algorithm::s() {
    return m_s;
}

int Algorithm::t() {
    return m_t;
}

int Algorithm::antCount() {
    return m_antCount;
}

qreal Algorithm::alpha() {
    return m_alpha;
}

qreal Algorithm::beta() {
    return m_beta;
}

qreal Algorithm::q() {
    return m_q;
}

qreal Algorithm::ro() {
    return m_ro;
}

qreal Algorithm::e() {
    return m_e;
}

QQmlListProperty<Path> Algorithm::shortestTripProperty() {
    return QQmlListProperty<Path>(this, m_shortestTrip);
}

void Algorithm::reset() {
    while (!m_ants.isEmpty()) {
        m_ants.first()->deleteLater();
        m_ants.removeFirst();
    }
    emit antsChanged();
    m_shortestTrip.clear();
    m_shortestTripLength = HUGE_VAL;
    emit shortestTripChanged();
    m_t = 0;
    m_s = 0;
    m_c = 0;
    emit sChanged();
    emit tChanged();
    emit cChanged();
    m_initialized = false;
    emit initializedChanged();
    for (Path *p : aco()->paths()) {
        p->setTrail(aco()->initialTau());
    }
}

void Algorithm::roundInit() {
    while (!m_ants.isEmpty()) {
        m_ants.first()->deleteLater();
        m_ants.removeFirst();
    }
    for (int i = 0; i < m_antCount && !aco()->towns().isEmpty(); i++) {
        m_ants.append(new Ant(this, aco()->towns()[qrand() % aco()->towns().size()]));
    }
    emit antsChanged();
    setInitialized(true);
}

void Algorithm::step() {
    if (!m_initialized)
        roundInit();

    for (Ant *a : m_ants) {
        a->step();
    }
    m_s++;
    emit sChanged();

    if (m_s >= aco()->towns().size()) {
        m_t += m_s;
        m_s = 0;
        m_c++;
        emit sChanged();
        emit tChanged();
        emit cChanged();
        qreal shortest = HUGE_VAL;
        int shortestPos = -1;
        for (int i = 0; i < m_ants.size(); i++) {
            m_ants[i]->setTown(m_ants[i]->firstTown());
            QList<Path*> trip = m_ants[i]->trip();
            qreal tripLength = m_ants[i]->tripLength();
            if (tripLength < shortest) {
                shortest = tripLength;
                shortestPos = i;
            }
            if (aco()->chosenAlgo() == Aco::AntCycle) {
                for (Path *p : trip) {
                    p->setTrail((1 - ro()) * p->trail() + q() / tripLength);
                }
            }
            else if (aco()->chosenAlgo() == Aco::AntDensity) {
                for (Path *p : trip) {
                    p->setTrail((1 - ro()) * p->trail() + q());
                }
            }
            else if (aco()->chosenAlgo() == Aco::AntQuantity) {
                for (Path *p : trip) {
                    p->setTrail((1 - ro()) * p->trail() + q() / p->distance());
                }
            }
        }
        if (shortest < m_shortestTripLength && m_ants[shortestPos]->trip().size() == aco()->towns().size()) {
            m_shortestTrip = m_ants[shortestPos]->trip();
            m_shortestTripLength = shortest;
            emit shortestTripChanged();
        }
        roundInit();
    }
}

void Algorithm::newAnt(Town *t) {
    setInitialized(false);
    m_ants.append(new Ant(this, t));
}

void Algorithm::setInitialized(bool i) {
    if (m_initialized != i) {
        m_initialized = i;
        emit initializedChanged();
    }
}

void Algorithm::setAntCount(int c) {
    if (m_antCount != c) {
        m_antCount = c;
        emit antCountChanged();
    }
}

void Algorithm::setAlpha(qreal alpha) {
    if (m_alpha != alpha) {
        m_alpha = alpha;
        emit alphaChanged();
    }
}

void Algorithm::setBeta(qreal beta) {
    if (m_beta != beta) {
        m_beta = beta;
        emit betaChanged();
    }
}

void Algorithm::setQ(qreal q) {
    if (m_q != q) {
        m_q = q;
        emit qChanged();
    }
}

void Algorithm::setRo(qreal ro) {
    if (m_ro != ro) {
        m_ro = ro;
        emit roChanged();
    }
}

void Algorithm::setE(qreal e) {
    if (m_e != e) {
        m_e = e;
        emit eChanged();
    }
}

void Algorithm::slotDistancesChanged() {
    if (m_shortestTrip.count() == 0)
        m_shortestTripLength = HUGE_VAL;
    else {
        m_shortestTripLength = 0.0;
        for (int i = 0; i < m_shortestTrip.size(); i++) {
            m_shortestTripLength += m_shortestTrip[i]->distance();
        }
    }
}

////////////////
//                  CANVAS
//

Canvas::Canvas(QObject *parent)
    : QObject(parent) {
    connect(this, &Canvas::pathsChanged, this, &Canvas::topologyChanged);
    connect(this, &Canvas::townsChanged, this, &Canvas::topologyChanged);
    connect(this, &Canvas::initialTauChanged, this, &Canvas::topologyChanged);
}

Town *Canvas::townAt(int x, int y) {
    QListIterator<Town*> it(m_towns);
    it.toBack();
    while (it.hasPrevious()) {
        Town *prev = it.previous();
        if (x - prev->x() < townSize() && x - prev->x() >= 0 && y - prev->y() < townSize() && y - prev->y() >= 0)
            return prev;
    }
    return nullptr;
}

Path *Canvas::pathBetween(Town *_a, Town *_b) {
    Town *a = _a, *b = _b;
    if (b < a) {
        a = _b;
        b = _a;
    }
    for (Path *p : m_paths)
        if (p->townA() == a && p->townB() == b)
            return p;
    return nullptr;
}

QList<Town *> &Canvas::towns() {
    return m_towns;
}

QQmlListProperty<Town> Canvas::townsListProperty() {
    return QQmlListProperty<Town>(this, m_towns);
}

QList<Path *> &Canvas::paths() {
    return m_paths;
}

QQmlListProperty<Path> Canvas::pathsListProperty() {
    return QQmlListProperty<Path>(this, m_paths);
}
int Canvas::townSize() {
    return m_townSize;
}

qreal Canvas::initialTau() {
    return m_initialTau;
}

qreal Canvas::animationSpeed() {
    return m_animationSpeed;
}

bool Canvas::fillPaths() {
    return m_fillPaths;
}

void Canvas::newTown(int x, int y) {
    Town *newTown = new Town(this, x, y);
    m_towns.append(newTown);
    emit townsChanged();
    if (m_fillPaths) {
        for (int i = 0; i < m_towns.size() - 1; i++) {
            addPath(newTown, m_towns[i]);
        }
    }
    emit pathsChanged();
}

void Canvas::deleteTown(Town *t) {
    QList<Path *> toDelete;
    for (Path *p : m_paths) {
        if (p->townA() == t || p->townB() == t) {
            toDelete.append(p);
        }
    }
    for (Path *p : toDelete) {
        disconnect(p, &Path::distanceChanged, this, &Canvas::distancesChanged);
        p->deleteLater();
        m_paths.removeOne(p);
    }
    if (!toDelete.isEmpty())
        emit pathsChanged();
    t->deleteLater();
    m_towns.removeOne(t);
    emit townsChanged();
}

void Canvas::togglePath(Town *a, Town *b) {
    if (m_fillPaths)
        return;

    if (!removePath(a, b))
        addPath(a, b);
}

bool Canvas::addPath(Town *_a, Town *_b) {
    Town *a = _a, *b = _b;
    if (!a || !b || a == b)
       return false;
    if (b < a) {
       a = _b;
       b = _a;
    }
    Path *tmp = new Path(this, a, b);
    connect(tmp, &Path::distanceChanged, this, &Canvas::distancesChanged);
    m_paths.append(tmp);
    emit pathsChanged();
    return true;
}

bool Canvas::removePath(Town *_a, Town *_b) {
    Town *a = _a, *b = _b;
    if (!a || !b || a == b)
        return false;
    if (b < a) {
        a = _b;
        b = _a;
    }
    Path *toDelete = nullptr;
    for (Path *p : m_paths) {
        if (p->townA() == a && p->townB() == b) {
            toDelete = p;
            break;
        }
    }
    if (toDelete) {
        disconnect(toDelete, &Path::distanceChanged, this, &Canvas::distancesChanged);
        toDelete->deleteLater();
        m_paths.removeOne(toDelete);
        emit pathsChanged();
        return true;
    }
    return false;
}

void Canvas::clear() {
    while (m_towns.size() > 0)
        deleteTown(m_towns.first());
}

void Canvas::setTownSize(int size) {
    if (m_townSize != size) {
        m_townSize = size;
        emit townSizeChanged();
    }
}

void Canvas::setInitialTau(qreal tau) {
    if (m_initialTau != tau) {
        m_initialTau = tau;
        emit initialTauChanged();
    }
}

void Canvas::setFillPaths(bool on) {
    if (m_fillPaths != on) {
        m_fillPaths = on;
        emit fillPaths();
    }
}

qreal Canvas::setAnimationSpeed(qreal newSpeed) {
    if (m_animationSpeed != newSpeed) {
        m_animationSpeed = newSpeed;
        emit animationSpeedChanged();
    }
    return m_animationSpeed;
}


////////////////
//                  ACO
//

Aco::Aco(QObject *parent)
    : Canvas(parent), m_currentAlgorithm(new Algorithm(this)) {
}

QString Aco::string() {
    QString res;
    for (Town *t : m_towns) {
        res.append(QString("%1;%2;%3\n").arg(t->x()).arg(t->y()).arg(t->name()));
    }
    for(Path *p : m_paths) {
        int a = m_towns.indexOf(p->townA());
        int b = m_towns.indexOf(p->townB());
        if (a >= 0 && b >= 0)
            res.append(QString("%1;%2;%3;%4\n").arg(a).arg(b).arg(p->distance()).arg(p->trail()));
    }
    return res;
}

void Aco::saveTo(const QUrl &file) {
    QFile f(file.toLocalFile());
    f.open(QIODevice::WriteOnly);
    f.write(string().toLocal8Bit());
    f.close();
}

void Aco::loadFrom(const QUrl &file) {
    Canvas::clear();
    QFile f(file.toLocalFile());
    f.open(QIODevice::ReadOnly);
    while (!f.atEnd()) {
        QString s = f.readLine();
        QStringList sL = s.split(";");
        if (sL.length() == 3) {
            newTown(sL[0].toInt(), sL[1].toInt());
            m_towns.last()->setName(sL[2]);
        }
        if (sL.length() == 4) {
            pathBetween(m_towns[sL[0].toInt()], m_towns[sL[1].toInt()])->setDistance(sL[2].toFloat());
        }
    }
    f.close();
}

qreal Aco::getRand() {
    return m_uniformDist(m_mersenneTwister);
}

Algorithm *Aco::algorithm() {
    return m_currentAlgorithm;
}

Canvas *Aco::canvas() {
    return this;
}

int Aco::chosenAlgo() {
    return m_chosenAlgo;
}

void Aco::setChosenAlgo(int a) {
    if (m_chosenAlgo != a) {
        qDebug() << "OFC";
        m_chosenAlgo = (Aco::Algorithms) a;
        emit chosenAlgoChanged();
    }
}
