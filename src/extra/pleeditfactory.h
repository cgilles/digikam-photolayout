/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2011-09-01
 * Description : a plugin to create photo layouts by fusion of several images.
 *
 * Copyright (C) 2011      by Lukasz Spas <lukasz dot spas at gmail dot com>
 * Copyright (C) 2009-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef PLE_EDIT_FACTORY_H
#define PLE_EDIT_FACTORY_H

// Qt includes

#include <QMap>
#include <QList>

// Local includes

#include "qteditorfactory.h"
#include "qtvariantproperty.h"
#include "qtpropertybrowser.h"

namespace PhotoLayoutsEditor
{

class QSpinBoxFactory : public QtSpinBoxFactory
{
    Q_OBJECT

public:

    explicit QSpinBoxFactory(QObject* const parent = nullptr);

Q_SIGNALS:

    void editingFinished();

protected:

    using QtSpinBoxFactory::createEditor;
    QWidget* createEditor(QtIntPropertyManager* manager, QtProperty* property, QWidget* parent) override;
};

// ---------------------------------------------------------------------

class QSliderFactory : public QtSliderFactory
{
    Q_OBJECT

public:

    explicit QSliderFactory(QObject* const parent = nullptr);

Q_SIGNALS:

    void editingFinished();

protected:

    using QtSliderFactory::createEditor;
    QWidget* createEditor(QtIntPropertyManager* manager, QtProperty* property, QWidget* parent) override;
};

// ---------------------------------------------------------------------

class QScrollBarFactory : public QtScrollBarFactory
{
    Q_OBJECT

public:

    explicit QScrollBarFactory(QObject* const parent = nullptr);

Q_SIGNALS:

    void editingFinished();

protected:

    using QtScrollBarFactory::createEditor;
    QWidget* createEditor(QtIntPropertyManager* manager, QtProperty* property, QWidget* parent) override;
};

// ---------------------------------------------------------------------

class QCheckBoxFactory : public QtCheckBoxFactory
{
    Q_OBJECT

public:

    explicit QCheckBoxFactory(QObject* const parent = nullptr);

Q_SIGNALS:

    void editingFinished();

protected:

    using QtCheckBoxFactory::createEditor;
    QWidget* createEditor(QtBoolPropertyManager* manager, QtProperty* property, QWidget* parent) override;
};

// ---------------------------------------------------------------------

class QDoubleSpinBoxFactory : public QtDoubleSpinBoxFactory
{
    Q_OBJECT

public:

    explicit QDoubleSpinBoxFactory(QObject* const parent = nullptr);

Q_SIGNALS:

    void editingFinished();

protected:

    using QtDoubleSpinBoxFactory::createEditor;
    QWidget* createEditor(QtDoublePropertyManager* manager, QtProperty* property, QWidget* parent) override;
};

// ---------------------------------------------------------------------

class QLineEditFactory : public QtLineEditFactory
{
    Q_OBJECT

public:

    explicit QLineEditFactory(QObject* const parent = nullptr);

Q_SIGNALS:

    void editingFinished();

protected:

    using QtLineEditFactory::createEditor;
    QWidget* createEditor(QtStringPropertyManager* manager, QtProperty* property, QWidget* parent) override;
};

// ---------------------------------------------------------------------

class QDateEditFactory : public QtDateEditFactory
{
    Q_OBJECT

public:

    explicit QDateEditFactory(QObject* const parent = nullptr);

Q_SIGNALS:

    void editingFinished();

protected:

    using QtDateEditFactory::createEditor;
    QWidget* createEditor(QtDatePropertyManager* manager, QtProperty* property, QWidget* parent) override;
};

// ---------------------------------------------------------------------

class QTimeEditFactory : public QtTimeEditFactory
{
    Q_OBJECT

public:

    explicit QTimeEditFactory(QObject* const parent = nullptr);

Q_SIGNALS:

    void editingFinished();

protected:

    using QtTimeEditFactory::createEditor;
    QWidget* createEditor(QtTimePropertyManager* manager, QtProperty* property, QWidget* parent) override;
};

// ---------------------------------------------------------------------

class QDateTimeEditFactory : public QtDateTimeEditFactory
{
    Q_OBJECT

public:

    explicit QDateTimeEditFactory(QObject* const parent = nullptr);

Q_SIGNALS:

    void editingFinished();

protected:

    using QtDateTimeEditFactory::createEditor;
    QWidget* createEditor(QtDateTimePropertyManager* manager, QtProperty* property, QWidget* parent) override;
};

// ---------------------------------------------------------------------

class QKeySequenceEditorFactory : public QtKeySequenceEditorFactory
{
    Q_OBJECT

public:

    explicit QKeySequenceEditorFactory(QObject* const parent = nullptr);

Q_SIGNALS:

    void editingFinished();

protected:

    using QtKeySequenceEditorFactory::createEditor;
    QWidget* createEditor(QtKeySequencePropertyManager* manager, QtProperty* property, QWidget* parent) override;
};

// ---------------------------------------------------------------------

class QCharEditorFactory : public QtCharEditorFactory
{
    Q_OBJECT

public:

    explicit QCharEditorFactory(QObject* const parent = nullptr);

Q_SIGNALS:

    void editingFinished();

protected:

    using QtCharEditorFactory::createEditor;
    QWidget* createEditor(QtCharPropertyManager* manager, QtProperty* property, QWidget* parent) override;
};

// ---------------------------------------------------------------------

class QEnumEditorFactory : public QtEnumEditorFactory
{
    Q_OBJECT

public:

    explicit QEnumEditorFactory(QObject* const parent = nullptr);

Q_SIGNALS:

    void editingFinished();

protected:

    using QtEnumEditorFactory::createEditor;
    QWidget* createEditor(QtEnumPropertyManager* manager, QtProperty* property, QWidget* parent) override;
};

// ---------------------------------------------------------------------

class QCursorEditorFactory : public QtCursorEditorFactory
{
    Q_OBJECT

public:

    explicit QCursorEditorFactory(QObject* const parent = nullptr);

Q_SIGNALS:

    void editingFinished();

protected:

    using QtCursorEditorFactory::createEditor;
    QWidget* createEditor(QtCursorPropertyManager* manager, QtProperty* property, QWidget* parent) override;
};

// ---------------------------------------------------------------------

class QColorEditorFactory : public QtColorEditorFactory
{
    Q_OBJECT

public:

    explicit QColorEditorFactory(QObject* const parent = nullptr);

Q_SIGNALS:

    void editingFinished();

protected:

    using QtColorEditorFactory::createEditor;
    QWidget* createEditor(QtColorPropertyManager* manager, QtProperty* property, QWidget* parent) override;
};

// ---------------------------------------------------------------------

class QFontEditorFactory : public QtFontEditorFactory
{
    Q_OBJECT

public:

    explicit QFontEditorFactory(QObject* const parent = nullptr);

Q_SIGNALS:

    void editingFinished();

protected:

    using QtFontEditorFactory::createEditor;
    QWidget* createEditor(QtFontPropertyManager* manager, QtProperty* property, QWidget* parent) override;
};

// ---------------------------------------------------------------------

class QVariantEditorFactory : public QtVariantEditorFactory
{
    Q_OBJECT

public:

    explicit QVariantEditorFactory(QObject* const parent = nullptr);

Q_SIGNALS:

    void editingFinished();

protected:

    using QtVariantEditorFactory::createEditor;
    QWidget* createEditor(QtVariantPropertyManager* manager, QtProperty* property, QWidget* parent) override;
};

// ---------------------------------------------------------------------

class QSliderEditFactory : public QtAbstractEditorFactory<QtIntPropertyManager>
{
    Q_OBJECT

public:

    explicit QSliderEditFactory(QObject* const parent = nullptr);

protected:

    virtual void connectPropertyManager(QtIntPropertyManager* manager) override;
    using QtAbstractEditorFactory<QtIntPropertyManager>::createEditor;
    virtual QWidget* createEditor(QtIntPropertyManager* manager, QtProperty* property, QWidget* parent) override;
    virtual void disconnectPropertyManager(QtIntPropertyManager* manager) override;

Q_SIGNALS:

    void editingFinished();

private Q_SLOTS:

    void slotEditorDestroyed(QObject*);

private:

    QtSliderFactory*                    originalFactory;
    QMap<QtProperty*, QList<QWidget*> > createdEditors;
    QMap<QWidget*, QtProperty*>         editorToProperty;
};

} // namespace PhotoLayoutsEditor

#endif // PLE_EDIT_FACTORY_H