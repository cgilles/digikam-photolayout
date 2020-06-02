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

#include "photolayoutswindow_p.h"

namespace PhotoLayoutsEditor
{

PhotoLayoutsWindow::PhotoLayoutsWindow(QWidget* const parent)
    : KXmlGuiWindow(parent),
      d(new Private)
{
    m_instance = this;

    initIconsResource();
    setXMLFile(QLatin1String(":/photolayoutseditorui.rc"));
    setWindowTitle(QObject::tr("Photo Layouts Editor"));
    setAttribute(Qt::WA_DeleteOnClose, true);

    loadEffects();
    loadBorders();
    setupActions();
    createWidgets();
    refreshActions();

    setAcceptDrops(true);
    int height              = QApplication::desktop()->height()*0.8;
    resize(qRound(height * (16.0 / 9.0)),height);
    QDesktopWidget* const d = qApp->desktop();
    move(d->rect().center() - this->frameGeometry().center());
}

PhotoLayoutsWindow::~PhotoLayoutsWindow()
{
    if (d->canvas)
    {
        d->canvas->deleteLater();
    }

    if (d)
    {
        delete d;
    }

    m_instance = 0;

    cleanupIconsResource();
}

PhotoLayoutsWindow* PhotoLayoutsWindow::instance(QWidget* const parent)
{
    if (m_instance)
    {
        return m_instance;
    }
    else
    {
        qApp->installEventFilter(new UndoCommandEventFilter(qApp));
        return (m_instance = new PhotoLayoutsWindow(parent));
    }
}

void PhotoLayoutsWindow::addUndoCommand(QUndoCommand* command)
{
    if (command)
    {
        qDebug() << command->text();

        if (d->canvas)
        {
            d->canvas->undoStack()->push(command);
        }
        else
        {
            command->redo();
            delete command;
        }
    }
}

void PhotoLayoutsWindow::beginUndoCommandGroup(const QString & name)
{
    if (d->canvas)
    {
        d->canvas->undoStack()->beginMacro(name);
    }
}

void PhotoLayoutsWindow::endUndoCommandGroup()
{
    if (d->canvas)
    {
        d->canvas->undoStack()->endMacro();
    }
}

void PhotoLayoutsWindow::setInterface(DInfoInterface* const interface)
{
    if (interface)
    {
        d->interface = interface;
    }
}

bool PhotoLayoutsWindow::hasInterface() const
{
    return (bool) d->interface;
}

DInfoInterface* PhotoLayoutsWindow::interface() const
{
    return this->d->interface;
}

void PhotoLayoutsWindow::setupActions()
{
    d->openNewFileAction = KStandardAction::openNew(this, SLOT(open()), actionCollection());
    actionCollection()->addAction(QLatin1String("open_new"), d->openNewFileAction);

    //------------------------------------------------------------------------

    d->openFileAction = new QAction(QObject::tr("Open Template File..."), actionCollection());
    connect(d->openFileAction, SIGNAL(triggered()), this, SLOT(openDialog()));
    actionCollection()->addAction(QLatin1String("open"), d->openFileAction);

    //------------------------------------------------------------------------

    d->saveAction = KStandardAction::save(this, SLOT(save()), actionCollection());
    actionCollection()->addAction(QLatin1String("save"), d->saveAction);

    //------------------------------------------------------------------------

    d->saveAsAction = KStandardAction::saveAs(this, SLOT(saveAs()), actionCollection());
    actionCollection()->setDefaultShortcut(d->saveAsAction, Qt::SHIFT + Qt::CTRL + Qt::Key_S);
    actionCollection()->addAction(QLatin1String("save_as"), d->saveAsAction);

    //------------------------------------------------------------------------

    d->saveAsTemplateAction = new QAction(QObject::tr("Saves canvas as a template file...", "Save As Template..."), actionCollection());
    connect(d->saveAsTemplateAction, SIGNAL(triggered()), this, SLOT(saveAsTemplate()));
    actionCollection()->addAction(QLatin1String("save_as_template"), d->saveAsTemplateAction);

    //------------------------------------------------------------------------

    d->exportFileAction = new QAction(QObject::tr("Export current frame layout to image file...", "Export..."), actionCollection());
    actionCollection()->setDefaultShortcut(d->exportFileAction, Qt::SHIFT + Qt::CTRL + Qt::Key_E);
    connect(d->exportFileAction, SIGNAL(triggered()), this, SLOT(exportFile()));
    actionCollection()->addAction(QLatin1String("export"), d->exportFileAction);

    //------------------------------------------------------------------------

    d->printPreviewAction = KStandardAction::printPreview(this, SLOT(printPreview()), actionCollection());
    actionCollection()->setDefaultShortcut(d->printPreviewAction, Qt::SHIFT + Qt::CTRL + Qt::Key_P);
    actionCollection()->addAction(QLatin1String("print_preview"), d->printPreviewAction);

    //------------------------------------------------------------------------

    d->printAction = KStandardAction::print(this, SLOT(print()), actionCollection());
    actionCollection()->addAction(QLatin1String("print"), d->printAction);

    //------------------------------------------------------------------------

    d->closeAction = KStandardAction::close(this, SLOT(closeDocument()), actionCollection());
    actionCollection()->addAction(QLatin1String("close"), d->closeAction);

    //------------------------------------------------------------------------

    d->quitAction = KStandardAction::quit(this, SLOT(close()), actionCollection());
    actionCollection()->addAction(QLatin1String("quit"), d->quitAction);

    //------------------------------------------------------------------------

    d->undoAction = KStandardAction::undo(0, 0, actionCollection());
    actionCollection()->addAction(QLatin1String("undo"), d->undoAction);

    //------------------------------------------------------------------------

    d->redoAction = KStandardAction::redo(0, 0, actionCollection());
    actionCollection()->addAction(QLatin1String("redo"), d->redoAction);

    //------------------------------------------------------------------------

    d->settingsAction = KStandardAction::preferences(this, SLOT(settings()), actionCollection());
    actionCollection()->addAction(QLatin1String("settings"), d->settingsAction);

    //------------------------------------------------------------------------

    d->addImageAction = new QAction(QObject::tr("Adds new image(s) from file...", "Add image(s)..."), actionCollection());
    connect(d->addImageAction, SIGNAL(triggered()), this, SLOT(loadNewImage()));
    actionCollection()->addAction(QLatin1String("new_image"), d->addImageAction);

    //------------------------------------------------------------------------

    d->showGridToggleAction = new KToggleAction(QObject::tr("View grid lines...", "Show..."), actionCollection());
    actionCollection()->setDefaultShortcut(d->showGridToggleAction, Qt::SHIFT + Qt::CTRL + Qt::Key_G);

    QSettings config;
    config.beginGroup(QLatin1String("View"));
    d->showGridToggleAction->setChecked(config.value(QLatin1String("ShowGrid"), false).toBool());
    config.endGroup();

    connect(d->showGridToggleAction, SIGNAL(triggered(bool)), this, SLOT(setGridVisible(bool)));
    actionCollection()->addAction(QLatin1String("grid_toggle"), d->showGridToggleAction);

    //------------------------------------------------------------------------

    d->gridConfigAction = new QAction(QObject::tr("Configure grid lines visibility...", "Setup grid..."), actionCollection());
    connect(d->gridConfigAction, SIGNAL(triggered()), this, SLOT(setupGrid()));
    actionCollection()->addAction(QLatin1String("grid_config"), d->gridConfigAction);

    //------------------------------------------------------------------------

    d->changeCanvasSizeAction = new QAction(QObject::tr("Configure canvas size...", "Change canvas size..."), actionCollection());
    connect(d->changeCanvasSizeAction, SIGNAL(triggered()), this, SLOT(changeCanvasSize()));
    actionCollection()->addAction(QLatin1String("canvas_size"), d->changeCanvasSizeAction);

    createGUI(xmlFile());
}

void PhotoLayoutsWindow::refreshActions()
{
    bool isEnabledForCanvas = false;

    if (d->canvas)
    {
        isEnabledForCanvas = true;
        d->undoAction->setEnabled(d->canvas->undoStack()->canUndo());
        d->redoAction->setEnabled(d->canvas->undoStack()->canRedo());
        d->saveAction->setEnabled(isEnabledForCanvas && !d->canvas->isSaved());
    }

    d->saveAsAction->setEnabled(isEnabledForCanvas);
    d->saveAsTemplateAction->setEnabled(isEnabledForCanvas);
    d->exportFileAction->setEnabled(isEnabledForCanvas);
    d->printPreviewAction->setEnabled(isEnabledForCanvas);
    d->printAction->setEnabled(isEnabledForCanvas);
    d->closeAction->setEnabled(isEnabledForCanvas);
    d->addImageAction->setEnabled(isEnabledForCanvas);
    d->showGridToggleAction->setEnabled(isEnabledForCanvas);
    d->gridConfigAction->setEnabled(isEnabledForCanvas);
    d->changeCanvasSizeAction->setEnabled(isEnabledForCanvas);
    d->treeWidget->setEnabled(isEnabledForCanvas);
    d->toolsWidget->setEnabled(isEnabledForCanvas);
}

void PhotoLayoutsWindow::createWidgets()
{
    // Tools
    d->toolsWidget = ToolsDockWidget::instance(this);
    this->addDockWidget(Qt::RightDockWidgetArea, d->toolsWidget);

    // Layers dockwidget
    d->treeWidget = new QDockWidget(QObject::tr("Layers"), this);
    d->treeWidget->setFeatures(QDockWidget::DockWidgetMovable);
    d->treeWidget->setFloating(false);
    d->treeWidget->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    d->tree = new LayersTree(d->treeWidget);
    d->tree->setAnimated(true);
    d->treeWidget->setWidget(d->tree);
    d->treeTitle = new LayersTreeTitleWidget(d->treeTitle);
    d->treeWidget->setTitleBarWidget(d->treeTitle);
    this->addDockWidget(Qt::LeftDockWidgetArea, d->treeWidget);
    d->treeWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    connect(d->toolsWidget,SIGNAL(requireMultiSelection()),d->tree,SLOT(setMultiSelection()));
    connect(d->toolsWidget,SIGNAL(requireSingleSelection()),d->tree,SLOT(setSingleSelection()));

    // Central widget (widget with canvas)
    d->centralWidget = new QWidget(this);
    d->centralWidget->setLayout(new QHBoxLayout(d->centralWidget));
    d->centralWidget->layout()->setContentsMargins(QMargins());
    d->centralWidget->layout()->setSpacing(0);
    setCentralWidget(d->centralWidget);

    d->statusBar = new PLEStatusBar(this);
    setStatusBar(d->statusBar);
}

void PhotoLayoutsWindow::createCanvas(const CanvasSize & size)
{
    if (d->canvas)
    {
        d->centralWidget->layout()->removeWidget(d->canvas);
        d->canvas->deleteLater();
    }

    d->canvas = new Canvas(size, d->centralWidget);
    this->prepareSignalsConnections();
}

void PhotoLayoutsWindow::createCanvas(const QUrl& fileUrl)
{
    if (d->canvas)
    {
        d->centralWidget->layout()->removeWidget(d->canvas);
        d->canvas->deleteLater();
    }

    QFile file(fileUrl.toLocalFile());
    QDomDocument document;
    document.setContent(&file, true);
    d->canvas = Canvas::fromSvg(document);

    if (d->canvas)
    {
        if (!d->canvas->isTemplate())
        {
            d->canvas->setFile(fileUrl);
        }

        d->canvas->setParent(d->centralWidget);
        this->prepareSignalsConnections();
    }
    else
    {
        QMessageBox::critical(this,
                              QObject::tr("Error"),
                              QObject::tr("Cannot read image file."));
    }
    file.close();
}

void PhotoLayoutsWindow::prepareSignalsConnections()
{
    d->centralWidget->layout()->addWidget(d->canvas);
    d->tree->setModel(d->canvas->model());
    d->tree->setSelectionModel(d->canvas->selectionModel());
    d->toolsWidget->setScene(d->canvas->scene());

    // undo stack signals
    connect(d->canvas,                              SIGNAL(savedStateChanged()),                this,                   SLOT(refreshActions()));
    connect(d->canvas->undoStack(),                 SIGNAL(canRedoChanged(bool)),               d->redoAction,          SLOT(setEnabled(bool)));
    connect(d->canvas->undoStack(),                 SIGNAL(canUndoChanged(bool)),               d->undoAction,          SLOT(setEnabled(bool)));
    connect(d->undoAction,                          SIGNAL(triggered()),                        d->canvas->undoStack(), SLOT(undo()));
    connect(d->redoAction,                          SIGNAL(triggered()),                        d->canvas->undoStack(), SLOT(redo()));

    // model/tree/canvas synchronization signals
    connect(d->tree,                                SIGNAL(selectedRowsAboutToBeRemoved()),     d->canvas,              SLOT(removeSelectedRows()));
    connect(d->tree,                                SIGNAL(selectedRowsAboutToBeMovedUp()),     d->canvas,              SLOT(moveSelectedRowsUp()));
    connect(d->tree,                                SIGNAL(selectedRowsAboutToBeMovedDown()),   d->canvas,              SLOT(moveSelectedRowsDown()));
    connect(d->treeTitle->moveUpButton(),           SIGNAL(clicked()),                          d->canvas,              SLOT(moveSelectedRowsUp()));
    connect(d->treeTitle->moveDownButton(),         SIGNAL(clicked()),                          d->canvas,              SLOT(moveSelectedRowsDown()));
    // interaction modes (tools)
    connect(d->canvas,                              SIGNAL(selectedItem(AbstractPhoto*)),       d->toolsWidget,         SLOT(itemSelected(AbstractPhoto*)));
    connect(d->toolsWidget,                         SIGNAL(undoCommandCreated(QUndoCommand*)),  d->canvas,              SLOT(newUndoCommand(QUndoCommand*)));
    connect(d->toolsWidget,                         SIGNAL(pointerToolSelected()),              d->canvas,              SLOT(enableDefaultSelectionMode()));
    connect(d->toolsWidget,                         SIGNAL(handToolSelected()),                 d->canvas,              SLOT(enableViewingMode()));
    connect(d->toolsWidget,                         SIGNAL(zoomToolSelected()),                 d->canvas,              SLOT(enableZoomingMode()));
    connect(d->toolsWidget,                         SIGNAL(canvasToolSelected()),               d->canvas,              SLOT(enableCanvasEditingMode()));
    connect(d->toolsWidget,                         SIGNAL(effectsToolSelected()),              d->canvas,              SLOT(enableEffectsEditingMode()));
    connect(d->toolsWidget,                         SIGNAL(textToolSelected()),                 d->canvas,              SLOT(enableTextEditingMode()));
    connect(d->toolsWidget,                         SIGNAL(rotateToolSelected()),               d->canvas,              SLOT(enableRotateEditingMode()));
    connect(d->toolsWidget,                         SIGNAL(scaleToolSelected()),                d->canvas,              SLOT(enableScaleEditingMode()));
    connect(d->toolsWidget,                         SIGNAL(cropToolSelected()),                 d->canvas,              SLOT(enableCropEditingMode()));
    connect(d->toolsWidget,                         SIGNAL(borderToolSelected()),               d->canvas,              SLOT(enableBordersEditingMode()));
    connect(d->toolsWidget,                         SIGNAL(newItemCreated(AbstractPhoto*)),     d->canvas,              SLOT(addNewItem(AbstractPhoto*)));
    connect(d->canvas->scene()->toGraphicsScene(),  SIGNAL(mousePressedPoint(QPointF)),         d->toolsWidget,         SLOT(mousePositionChoosen(QPointF)));

    d->toolsWidget->setDefaultTool();
}

void PhotoLayoutsWindow::open()
{
    NewCanvasDialog* const dialog = new NewCanvasDialog(this);
    dialog->setModal(true);

    int result = dialog->exec();

    if (result != QDialog::Accepted)
    {
        return;
    }

    QString tmp;

    if (dialog->hasTemplateSelected() && !(tmp = dialog->templateSelected()).isEmpty())
    {
        open(QUrl(dialog->templateSelected()));
    }
    else
    {
        CanvasSize size = dialog->canvasSize();

        if (size.isValid())
        {
            closeDocument();
            createCanvas(size);
            refreshActions();
        }
    }

    delete dialog;
}

void PhotoLayoutsWindow::openDialog()
{
    if (!d->fileDialog)
    {
        d->fileDialog = new QFileDialog(this,
                                        QString(),
                                        QString(),
                                        QObject::tr("*.ple|Photo Layouts Editor files"));
    }

    d->fileDialog->setFileMode(QFileDialog::AnyFile);

    int result = d->fileDialog->exec();

    if (result == QFileDialog::Accepted &&
        !d->fileDialog->selectedUrls().isEmpty())
    {
        QUrl url = d->fileDialog->selectedUrls().first();
        qDebug() << url;
        open(url);
    }
}

void PhotoLayoutsWindow::open(const QUrl& fileUrl)
{
    if (d->canvas && (d->canvas->file() == fileUrl))
    {
        return;
    }

    if (fileUrl.isValid())
    {
        closeDocument();
        createCanvas(fileUrl);
        refreshActions();
    }
}

void PhotoLayoutsWindow::save()
{
    qDebug() << !d->canvas->file().isValid() << d->canvas->file().fileName().isEmpty() << d->canvas->isTemplate();

    if (!d->canvas)
    {
        return;
    }

    if (!d->canvas->file().isValid() || d->canvas->file().fileName().isEmpty() || d->canvas->isTemplate())
    {
        saveAs();
    }
    else
    {
        saveFile();
    }
}

void PhotoLayoutsWindow::saveAs()
{
    if (!d->fileDialog)
    {
        d->fileDialog = new QFileDialog(this,
                                        QString(),
                                        QString(),
                                        QObject::tr("*.ple|Photo Layouts Editor files"));
    }

    d->fileDialog->setFileMode(QFileDialog::AnyFile);

    int result = d->fileDialog->exec();

    if ((result == QFileDialog::Accepted) &&
        !d->fileDialog->selectedUrls().isEmpty())
    {
        saveFile(d->fileDialog->selectedUrls().first());
    }
}

void PhotoLayoutsWindow::saveAsTemplate()
{
    if (!d->fileDialog)
    {
        d->fileDialog = new QFileDialog(this,
                                        QString(),
                                        QString(),
                                        QObject::tr("*.ple|Photo Layouts Editor files"));
    }

    d->fileDialog->setFileMode(QFileDialog::AnyFile);

    int result = d->fileDialog->exec();

    if ((result == QFileDialog::Accepted) &&
        !d->fileDialog->selectedUrls().isEmpty())
    {
        QUrl url = d->fileDialog->selectedUrls().first();

        if (d->canvas)
        {
            d->canvas->saveTemplate(url);
        }
        else
        {
            QMessageBox::critical(this,
                                  QObject::tr("Error"),
                                  QObject::tr("There is nothing to save."));
        }
    }
}

void PhotoLayoutsWindow::saveFile(const QUrl & fileUrl, bool setFileAsDefault)
{
    if (d->canvas)
    {
        d->canvas->save(fileUrl, setFileAsDefault);
    }
    else
    {
        QMessageBox::critical(this,
                              QObject::tr("Error"),
                              QObject::tr("There is nothing to save."));
    }
}

void PhotoLayoutsWindow::exportFile()
{
    if (!d->canvas)
    {
        return;
    }

    QString all;
    QStringList list                       = supportedImageMimeTypes(QIODevice::WriteOnly, all);
    QFileDialog* const imageFileSaveDialog = new QFileDialog(this);
    imageFileSaveDialog->setWindowTitle(QObject::tr("New Image File Name"));
    imageFileSaveDialog->setAcceptMode(QFileDialog::AcceptSave);
    imageFileSaveDialog->setFileMode(QFileDialog::AnyFile);
    imageFileSaveDialog->setNameFilters(list);

    int result       = imageFileSaveDialog->exec();
    QList<QUrl> urls = imageFileSaveDialog->selectedUrls();
    QString ext      = imageFileSaveDialog->selectedNameFilter().section(QLatin1String("*."), 1, 1);
    ext              = ext.left(ext.length() - 1);

    if (result == QFileDialog::Accepted && !urls.isEmpty() && !ext.isEmpty())
    {
        QUrl url = urls.first();

        QPixmap image(d->canvas->sceneRect().size().toSize());
        image.fill(Qt::transparent);
        d->canvas->renderCanvas(&image);
        QImageWriter writer(url.toLocalFile());
        writer.setFormat(ext.toLatin1());

        if (!writer.canWrite())
        {
            QMessageBox::critical(this, QObject::tr("Error"),
                                  QObject::tr("Image can't be saved in selected file."));
        }

        if (!writer.write(image.toImage()))
        {
            DMessageBox::showInformationList(
                QMessageBox::Critical,
                qApp->activeWindow(),
                qApp->applicationName(),
                QObject::tr("Unexpected error while saving an image."),
                QStringList() << writer.errorString());
        }
    }

    delete imageFileSaveDialog;
}

void PhotoLayoutsWindow::printPreview()
{
    if (d->canvas && d->canvas->scene())
    {
        QPrinter* const printer           = new QPrinter();
        d->canvas->preparePrinter(printer);
        QPrintPreviewDialog* const dialog = new QPrintPreviewDialog(printer, this);

        connect(dialog, SIGNAL(paintRequested(QPrinter*)),
                d->canvas, SLOT(renderCanvas(QPrinter*)));

        dialog->exec();
        delete dialog;
        delete printer;
    }
}

void PhotoLayoutsWindow::print()
{
    QPrinter* const printer    = new QPrinter();
    d->canvas->preparePrinter(printer);
    QPrintDialog* const dialog = new QPrintDialog(printer, this);

    connect(dialog, SIGNAL(accepted(QPrinter*)),
            d->canvas, SLOT(renderCanvas(QPrinter*)));

    dialog->exec();
    delete dialog;
    delete printer;
}

bool PhotoLayoutsWindow::closeDocument()
{
    if (d->canvas)
    {
        // Try to save unsaved changes
        int saving = QMessageBox::No;

        if (!d->canvas->isSaved())
        {
            saving = QMessageBox::question(this,
                                           QObject::tr("Save"),
                                           QObject::tr("Save changes to current frame?"),
                                           QMessageBox::StandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel));
        }

        switch (saving)
        {
            case QMessageBox::Yes:
                save();

            case QMessageBox::No:
                d->tree->setModel(0);
                d->canvas->deleteLater();
                d->canvas = nullptr;
                refreshActions();
                return true;

            default:
                return false;
        }
    }

    refreshActions();
    return true;
}

void PhotoLayoutsWindow::progressEvent(ProgressEvent* event)
{
    if (d->canvas)
    {
        d->canvas->progressEvent(event);
    }
}

bool PhotoLayoutsWindow::queryClose()
{
    if (closeDocument())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void PhotoLayoutsWindow::settings()
{
    QPointer<PLEConfigDialog> const dialog = new PLEConfigDialog(this);
    dialog->show();
    
    QSettings config;
    config.beginGroup(QLatin1String("View"));
    d->canvas->setAntialiasing(config.value(QLatin1String("Antialiasing"), false).toBool());
    d->canvas->scene()->setGridVisible(config.value(QLatin1String("ShowGrid"), false).toBool());
    d->canvas->scene()->setHorizontalGrid(config.value(QLatin1String("XGrid"), 25.0).toDouble());
    d->canvas->scene()->setVerticalGrid(config.value(QLatin1String("YGrid"), 25.0).toDouble());
    config.endGroup();
}

void PhotoLayoutsWindow::loadImages(const QList<QUrl>& urls)
{
    if (!d->canvas)
    {
        return;
    }

    if (!urls.isEmpty())
    {
        d->canvas->addImages(urls);
    }
}

void PhotoLayoutsWindow::loadNewImage()
{
    QList<QUrl> urls = ImageDialog::getImageURLs(this, QUrl());

    loadImages(urls);
}

void PhotoLayoutsWindow::setGridVisible(bool isVisible)
{
    d->showGridToggleAction->setChecked(isVisible);
    
    QSettings config;
    config.beginGroup(QLatin1String("View"));
    config.setValue(QLatin1String("ShowGrid"), isVisible);
    config.endGroup();
    config.sync();
    
    if (d->canvas && d->canvas->scene())
    {
        d->canvas->scene()->setGridVisible(isVisible);
    }
}

void PhotoLayoutsWindow::setupGrid()
{
    if (d->canvas && d->canvas->scene())
    {
        GridSetupDialog* const dialog = new GridSetupDialog(this);
        dialog->setHorizontalDistance(d->canvas->scene()->gridHorizontalDistance());
        dialog->setVerticalDistance(d->canvas->scene()->gridVerticalDistance());
        dialog->exec();
        d->canvas->scene()->setGrid(dialog->horizontalDistance(),
                                   dialog->verticalDistance());
        delete dialog;
    }
}

void PhotoLayoutsWindow::changeCanvasSize()
{
    if (!d->canvas)
    {
        return;
    }

    CanvasSizeDialog* const ccd = new CanvasSizeDialog(d->canvas->canvasSize(), this);
    int result                  = ccd->exec();
    CanvasSize size             = ccd->canvasSize();

    if (result == QDialog::Accepted)
    {
        if (size.isValid())
        {
            if (d->canvas->canvasSize() != size)
            {
                CanvasSizeChangeCommand* const command = new CanvasSizeChangeCommand(size, d->canvas);
                PLE_PostUndoCommand(command);
            }
        }
        else
        {
            QMessageBox::critical(this, QObject::tr("Error"), QObject::tr("Invalid image size."));
        }
    }

    delete ccd;
}

void PhotoLayoutsWindow::setTemplateEditMode(bool isEnabled)
{
    Q_UNUSED(isEnabled);

    //d->toolsWidget->setTemplateEditMode(isEnabled);
    //d->treeWidget->setTemplateEditMode(isEnabled);
    //d->canvas->setTemplateEditMode(isEnabled);
}

void PhotoLayoutsWindow::loadEffects()
{
    StandardEffectsFactory* const stdEffects = new StandardEffectsFactory(PhotoEffectsLoader::instance());
    PhotoEffectsLoader::registerEffect(stdEffects);
}

void PhotoLayoutsWindow::loadBorders()
{
    StandardBordersFactory* const stdBorders = new StandardBordersFactory(BorderDrawersLoader::instance());
    BorderDrawersLoader::registerDrawer(stdBorders);
}

} // namespace PhotoLayoutsEditor
