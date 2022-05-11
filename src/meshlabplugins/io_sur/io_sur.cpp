/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2006                                          \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#define IOSUR_VERSION "v1.0.2"

/* SUR file IO
 * Jason Cipriani <jason.cipriani@smith-nephew.com>
 * Copyright (C) 2022 Smith+Nephew
 *
 * CHANGES
 *
 *     v1.0.2 - 2022-May-11: temporarily disable comments in output
 *     v1.0.1 - 2022-Apr-23: fix hang on unexpected input eof
 *     v1.0.0 - 2022-Apr-22: initial version
 *
 * FILE FORMAT
 *
 *        vertex_count
 *        x y z
 *        ...
 *        triangle_count
 *        a b c
 *        ...
 *
 *    Blank lines and lines starting with # are ignored.
 *    Triangle vertex indices are 0-based.
 */

#include "io_sur.h"
#include <Qt>
#include <QDateTime>

using namespace vcg;

static void parseSUR(QString filename, CMeshO &m);
static void writeSUR(QString filename, CMeshO &m, QString name, bool comments);

void SurIOPlugin::open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask, const RichParameterList &parlst, CallBackPos * /*cb*/)
{
    if(formatName.toUpper() == tr("SUR"))
        parseSUR(fileName, m.cm);
    else
        wrongOpenFormat(formatName);
}

void SurIOPlugin::save(const QString & formatName, const QString &fileName, MeshModel &m, const int /*mask*/, const RichParameterList &, vcg::CallBackPos * /*cb*/)
{
    if (formatName.toUpper() == tr("SUR"))
        writeSUR(fileName, m.cm, QFileInfo(fileName).baseName(), false);
    else
        wrongSaveFormat(formatName);
}

QString SurIOPlugin::pluginName() const
{
    return "IOSUR-" IOSUR_VERSION;
}

QString SurIOPlugin::vendor() const {
    return "Smith+Nephew";
}

std::list<FileFormat> SurIOPlugin::importFormats() const
{
    return {FileFormat("SUR (the one and only)", tr("SUR"))};
}

std::list<FileFormat> SurIOPlugin::exportFormats() const
{
    return {FileFormat("SUR (the one and only)", tr("SUR"))};
}

void SurIOPlugin::exportMaskCapability(const QString &formatName, int &capability, int &defaultBits) const
{
    Q_UNUSED(formatName);
    capability = defaultBits = 0;
}
 

void writeSUR(QString filename, CMeshO &m, QString name, bool comments) {

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        throw MLException("Failed to open SUR file: " + file.errorString());

    tri::Allocator<CMeshO>::CompactVertexVector(m);
    tri::Allocator<CMeshO>::CompactFaceVector(m);

    if (comments) {
        file.write(QString("# %1\n").arg(name).toUtf8());
        file.write(QString("# Saved with MeshLab (IOSUR %2) on %1\n").arg(QDateTime::currentDateTime().toString(), IOSUR_VERSION).toUtf8());
    }

    file.write(QString("%1\n").arg(m.vert.size()).toLatin1());

    for (const auto &vert : m.vert) {
        const auto &pos = vert.cP();
        //file.write(QString("%1 %2 %3\n").arg(pos[0]).arg(pos[1]).arg(pos[2]).toLatin1());
        file.write(QString::asprintf("%f %f %f\n", pos[0], pos[1], pos[2]).toLatin1());
    }

    file.write(QString("%1\n").arg(m.face.size()).toLatin1());

    // io_json uses pointer math to compute vertex index so, i guess
    // we will too.
    const auto *v0 = &(m.vert[0]);
    for (const auto &face : m.face) {
        // todo: do i need to do something special for quads... ?
        // will there even be any at this point?
        int a = (face.cV(0) - v0);
        int b = (face.cV(1) - v0);
        int c = (face.cV(2) - v0);
        file.write(QString("%1 %2 %3\n").arg(a).arg(b).arg(c).toLatin1());
    }

}


void parseSUR(QString filename, CMeshO &m) {

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw MLException("Failed to open SUR file: " + file.errorString());

    enum { VCount, VData, TCount, TData, Complete } state = VCount;
    unsigned linenum = 0, vcount = 0, tcount = 0, vread = 0, tread = 0;
    const QRegExp space("\\s+");

    while (!file.atEnd()) {
        ++ linenum;
        QString line = QString::fromLatin1(file.readLine()).trimmed();
        if (file.error() != QFile::NoError)
            throw MLException("Error reading SUR file: " + file.errorString());
        if (line.isEmpty() || line.startsWith('#'))
            continue;
        QStringList items = line.split(space, Qt::SkipEmptyParts);
        bool ok = false;
        if (state == VCount && items.size() == 1) {
            vcount = items[0].toUInt(&ok);
            if (ok) {
                tri::Allocator<CMeshO>::AddVertices(m, vcount);
                state = vcount ? VData : TCount;
            }
        } else if (state == VData && items.size() == 3) {
            double x = items[0].toDouble(&ok);
            double y = ok ? items[1].toDouble(&ok) : 0;
            double z = ok ? items[2].toDouble(&ok) : 0;
            if (ok) {
                m.vert[vread].P() = Point3m(x, y, z);
                if (++ vread >= vcount)
                    state = TCount;
            }
        } else if (state == TCount && items.size() == 1) {
            tcount = items[0].toUInt(&ok);
            if (ok) {
                tri::Allocator<CMeshO>::AddFaces(m, tcount);
                state = tcount ? TData : Complete;
            }
        } else if (state == TData && items.size() == 3) {
            unsigned a = items[0].toUInt(&ok);
            unsigned b = ok ? items[1].toUInt(&ok) : 0;
            unsigned c = ok ? items[2].toUInt(&ok) : 0;
            ok = ok && (a < vcount) && (b < vcount) && (c < vcount);
            if (ok) {
                m.face[tread].V(0) = &m.vert[a];
                m.face[tread].V(1) = &m.vert[b];
                m.face[tread].V(2) = &m.vert[c];
                if (++ tread >= tcount)
                    state = Complete;
            }
        } // else if state == Complete then we've just read trailing garbage (!ok)
        if (!ok)
            throw MLException(QString("Error parsing SUR file, line %1").arg(linenum));
    }

    if (state != Complete)
        throw MLException(QString("Error parsing SUR file, unexpected end of file."));

}

MESHLAB_PLUGIN_NAME_EXPORTER(SurIOPlugin)