#include "BLL/route_service.h"
#include "DAL/text_map_repository.h"

#include <QApplication>
#include <QAbstractItemView>
#include <QComboBox>
#include <QColor>
#include <QDialog>
#include <QDoubleValidator>
#include <QElapsedTimer>
#include <QFormLayout>
#include <QFrame>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QLineF>
#include <QMainWindow>
#include <QMessageBox>
#include <QPainter>
#include <QPushButton>
#include <QScrollArea>
#include <QTableWidget>
#include <QTabWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QStringList>

#include <algorithm>
#include <cmath>
#include <exception>
#include <functional>
#include <limits>
#include <string>
#include <vector>

namespace {
const QColor backgroundColor("#0f172a");
const QColor panelColor("#19243a");
const QColor roadColor("#4b5b74");
const QColor blueColor("#3b82f6");
const QColor pickupColor("#f472b6");
const QColor greenColor("#22c55e");
const QColor redColor("#ef4444");
const QColor yellowColor("#facc15");
const QColor mutedColor("#a4b2c7");

QString qString(const std::string& value) {
    return QString::fromUtf8(value.c_str());
}

QString number(double value) {
    return QString::number(value, 'f', 2);
}

bool parsePositiveInt(const QString& text, int& value) {
    bool ok = false;
    value = text.trimmed().toInt(&ok);
    return ok && value > 0;
}

bool parseFiniteDouble(const QString& text, double& value) {
    bool ok = false;
    value = text.trimmed().toDouble(&ok);
    return ok && std::isfinite(value);
}

enum class EntityType { Intersection, Road };
enum class FormAction { Add, Edit };

struct ManagementPage {
    QTableWidget* table = nullptr;
    std::vector<QLineEdit*> fields;
    FormAction action = FormAction::Add;
};

class MapWidget final : public QWidget {
public:
    explicit MapWidget(const MapView& mapView, QWidget* parent = nullptr)
        : QWidget(parent), mapView(mapView) {
        setMinimumSize(520, 500);
        setMouseTracking(true);
        setFocusPolicy(Qt::StrongFocus);
    }

    void setSelectionIds(int taxiId, int pickupId, int destinationId) {
        this->taxiId = taxiId;
        this->pickupId = pickupId;
        this->destinationId = destinationId;
        update();
    }

    void setRoute(const std::vector<int>& intersections,
                  const std::vector<int>& roads,
                  size_t firstLegEdgeCount) {
        routeIntersections = intersections;
        routeRoads = roads;
        this->firstLegEdgeCount = firstLegEdgeCount;
        update();
    }

    void clearRoute() {
        routeIntersections.clear();
        routeRoads.clear();
        firstLegEdgeCount = 0;
        taxiVisible = false;
        update();
    }

    void setAnimationProgress(double progress) {
        animationProgress = std::clamp(progress, 0.0, 1.0);
        taxiVisible = !routeIntersections.empty();
        update();
    }

    std::function<void(int)> intersectionClicked;
    int selectionTarget = 0;
    bool taxiRunning = false;

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.fillRect(rect(), backgroundColor);

        const QRectF canvas = rect().adjusted(12, 12, -12, -12);
        painter.setPen(Qt::NoPen);
        painter.setBrush(panelColor);
        painter.drawRoundedRect(canvas, 14, 14);

        const QRectF mapRect = canvas.adjusted(20, 20, -20, -20);
        painter.save();
        painter.setClipRect(mapRect);

        for (const RoadView& road : mapView.roads) {
            const QPointF start = pointFor(road.startId, mapRect);
            const QPointF end = pointFor(road.endId, mapRect);
            bool firstLeg = false;
            bool secondLeg = false;
            for (size_t i = 0; i < routeRoads.size(); ++i) {
                if (routeRoads[i] == road.id) {
                    if (i < firstLegEdgeCount) firstLeg = true;
                    else secondLeg = true;
                }
            }

            const QColor color = firstLeg ? blueColor : (secondLeg ? yellowColor : roadColor);
            QPen pen(color, firstLeg || secondLeg ? 4.0 : 1.6);
            painter.setPen(pen);
            painter.drawLine(start, end);
            if (firstLeg || secondLeg) {
                painter.setPen(color);
                painter.drawText((start + end) / 2 + QPointF(4, -5), number(road.distance));
            }
        }

        for (const IntersectionView& node : mapView.intersections) {
            const QPointF point = pointFor(node.id, mapRect);
            QColor color = blueColor;
            if (node.id == taxiId) color = greenColor;
            if (node.id == pickupId) color = pickupColor;
            if (node.id == destinationId) color = redColor;
            if (std::find(routeIntersections.begin(), routeIntersections.end(), node.id) !=
                    routeIntersections.end() &&
                node.id != taxiId && node.id != pickupId && node.id != destinationId) {
                color = yellowColor;
            }

            const bool named = !node.name.empty();
            const qreal radius = named || node.id == taxiId || node.id == pickupId ||
                    node.id == destinationId
                ? 9.0
                : 4.0;
            painter.setPen(QPen(Qt::white, 1));
            painter.setBrush(named || node.id == taxiId || node.id == pickupId ||
                                     node.id == destinationId
                                 ? color
                                 : mutedColor);
            painter.drawEllipse(point, radius, radius);
            if (named) {
                painter.setPen(Qt::white);
                painter.drawText(point + QPointF(12, -5),
                                 QString::number(node.id) + " " + qString(node.name));
            }
        }

        if (taxiRunning && taxiVisible && routeIntersections.size() > 1) {
            const double segmentPosition = std::min(
                animationProgress * static_cast<double>(routeIntersections.size() - 1),
                static_cast<double>(routeIntersections.size() - 1) - 0.0001);
            const size_t segment = static_cast<size_t>(std::max(0.0, std::floor(segmentPosition)));
            const qreal amount = segmentPosition - static_cast<double>(segment);
            const QPointF start = pointFor(routeIntersections[segment], mapRect);
            const QPointF end = pointFor(
                routeIntersections[std::min(segment + 1, routeIntersections.size() - 1)],
                mapRect);
            const QPointF taxiPoint = start + (end - start) * amount;
            painter.setPen(QPen(Qt::black, 1));
            painter.setBrush(yellowColor);
            painter.drawEllipse(taxiPoint, 7, 7);
        }

        painter.restore();
        painter.setPen(mutedColor);
        painter.drawText(QRectF(24, height() - 30, width() - 48, 20),
                         Qt::AlignLeft | Qt::AlignVCenter,
                         "Lan chuot de zoom | Keo chuot phai de di chuyen ban do");
    }

    void wheelEvent(QWheelEvent* event) override {
        const QPointF anchor = event->position();
        const qreal nextZoom = std::clamp(
            zoom * std::pow(1.2, event->angleDelta().y() / 120.0), 0.75, 4.0);
        const qreal ratio = nextZoom / zoom;
        const QPointF center = rect().center();
        pan = anchor - center - (anchor - center - pan) * ratio;
        zoom = nextZoom;
        update();
        event->accept();
    }

    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::RightButton) {
            dragging = true;
            lastMousePosition = event->position();
            return;
        }
        if (event->button() != Qt::LeftButton || taxiRunning) return;

        const QRectF mapRect = rect().adjusted(32, 32, -32, -32);
        qreal nearestDistance = std::numeric_limits<qreal>::max();
        int nearestId = -1;
        for (const IntersectionView& node : mapView.intersections) {
            const QPointF point = pointFor(node.id, mapRect);
            const qreal distance = QLineF(point, event->position()).length();
            if (distance <= 18 && distance < nearestDistance) {
                nearestDistance = distance;
                nearestId = node.id;
            }
        }
        if (nearestId != -1 && intersectionClicked) intersectionClicked(nearestId);
    }

    void mouseMoveEvent(QMouseEvent* event) override {
        if (dragging) {
            pan += event->position() - lastMousePosition;
            lastMousePosition = event->position();
            update();
        }
    }

    void mouseReleaseEvent(QMouseEvent* event) override {
        if (event->button() == Qt::RightButton) dragging = false;
    }

private:
    QPointF pointFor(int id, const QRectF& mapRect) const {
        const auto found = std::find_if(
            mapView.intersections.begin(), mapView.intersections.end(),
            [id](const IntersectionView& item) { return item.id == id; });
        if (found == mapView.intersections.end()) return QPointF();

        double minX = found->x;
        double maxX = found->x;
        double minY = found->y;
        double maxY = found->y;
        for (const IntersectionView& item : mapView.intersections) {
            minX = std::min(minX, item.x);
            maxX = std::max(maxX, item.x);
            minY = std::min(minY, item.y);
            maxY = std::max(maxY, item.y);
        }
        const double xRange = std::max(1.0, maxX - minX);
        const double yRange = std::max(1.0, maxY - minY);
        const QPointF base(
            mapRect.left() + 24 + (found->x - minX) / xRange * (mapRect.width() - 48),
            mapRect.bottom() - 24 - (found->y - minY) / yRange * (mapRect.height() - 48));
        const QPointF center = mapRect.center();
        return center + (base - center) * zoom + pan;
    }

    const MapView& mapView;
    std::vector<int> routeIntersections;
    std::vector<int> routeRoads;
    size_t firstLegEdgeCount = 0;
    int taxiId = -1;
    int pickupId = -1;
    int destinationId = -1;
    qreal zoom = 1.0;
    QPointF pan;
    QPointF lastMousePosition;
    double animationProgress = 0.0;
    bool taxiVisible = false;
    bool dragging = false;
};

class ManagementDialog final : public QDialog {
public:
    explicit ManagementDialog(RouteService& routeService, QWidget* parent = nullptr)
        : QDialog(parent), routeService(routeService) {
        setWindowTitle("Quan ly giao lo va duong");
        resize(900, 620);
        auto* layout = new QVBoxLayout(this);
        auto* tabs = new QTabWidget(this);
        layout->addWidget(tabs);
        QWidget* intersectionTab = createPage(EntityType::Intersection, tabs);
        QWidget* roadTab = createPage(EntityType::Road, tabs);
        tabs->addTab(intersectionTab, "Giao lo");
        tabs->addTab(roadTab, "Duong");

        auto* closeButton = new QPushButton("Dong", this);
        closeButton->setObjectName("secondaryButton");
        connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
        layout->addWidget(closeButton, 0, Qt::AlignRight);
        refreshPage(EntityType::Intersection);
        refreshPage(EntityType::Road);
    }

private:
    QWidget* createPage(EntityType type, QTabWidget*) {
        auto* pageWidget = new QWidget;
        auto* layout = new QHBoxLayout(pageWidget);
        auto* table = new QTableWidget(pageWidget);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setSelectionMode(QAbstractItemView::SingleSelection);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->horizontalHeader()->setStretchLastSection(true);
        table->verticalHeader()->setVisible(false);
        layout->addWidget(table, 3);

        auto* editorPanel = new QWidget(pageWidget);
        auto* editorLayout = new QVBoxLayout(editorPanel);
        auto* form = new QFormLayout;
        ManagementPage& page = type == EntityType::Intersection
            ? intersectionPage
            : roadPage;
        page.table = table;
        const QStringList labels = type == EntityType::Intersection
            ? QStringList{"ID giao lo", "Ten", "Toa do X", "Toa do Y"}
            : QStringList{"ID duong", "ID dau", "ID cuoi", "Thoi gian", "Khoang cach"};
        for (const QString& label : labels) {
            auto* field = new QLineEdit(editorPanel);
            if (label != "Ten") {
                field->setValidator(new QDoubleValidator(field));
            }
            page.fields.push_back(field);
            form->addRow(label, field);
        }
        editorLayout->addLayout(form);

        auto* addButton = new QPushButton("Them moi", editorPanel);
        auto* editButton = new QPushButton("Cap nhat", editorPanel);
        auto* saveButton = new QPushButton("Luu thay doi", editorPanel);
        auto* deleteButton = new QPushButton("Xoa", editorPanel);
        deleteButton->setObjectName("dangerButton");
        editorLayout->addWidget(addButton);
        editorLayout->addWidget(editButton);
        editorLayout->addWidget(saveButton);
        editorLayout->addWidget(deleteButton);
        editorLayout->addStretch();
        layout->addWidget(editorPanel, 2);

        connect(table, &QTableWidget::itemSelectionChanged, this, [this, type]() {
            loadSelected(type);
        });
        connect(addButton, &QPushButton::clicked, this, [this, type]() {
            setAction(type, FormAction::Add);
            clearFields(type);
        });
        connect(editButton, &QPushButton::clicked, this, [this, type]() {
            ManagementPage& current = pageFor(type);
            if (current.table->currentRow() < 0) {
                QMessageBox::information(this, "Chua chon", "Hay chon mot dong trong danh sach.");
                return;
            }
            setAction(type, FormAction::Edit);
            loadSelected(type);
        });
        connect(saveButton, &QPushButton::clicked, this, [this, type]() { save(type); });
        connect(deleteButton, &QPushButton::clicked, this, [this, type]() {
            removeSelected(type);
        });
        return pageWidget;
    }

    ManagementPage& pageFor(EntityType type) {
        return type == EntityType::Intersection ? intersectionPage : roadPage;
    }

    void setAction(EntityType type, FormAction action) {
        pageFor(type).action = action;
    }

    void clearFields(EntityType type) {
        for (QLineEdit* field : pageFor(type).fields) field->clear();
        pageFor(type).table->clearSelection();
    }

    void loadSelected(EntityType type) {
        ManagementPage& page = pageFor(type);
        const int row = page.table->currentRow();
        if (row < 0) return;
        const int id = page.table->item(row, 0)->text().toInt();
        if (type == EntityType::Intersection) {
            const auto& items = routeService.getMapView().intersections;
            const auto found = std::find_if(items.begin(), items.end(),
                [id](const IntersectionView& item) { return item.id == id; });
            if (found == items.end()) return;
            page.fields[0]->setText(QString::number(found->id));
            page.fields[1]->setText(qString(found->name));
            page.fields[2]->setText(number(found->x));
            page.fields[3]->setText(number(found->y));
        } else {
            const auto& items = routeService.getMapView().roads;
            const auto found = std::find_if(items.begin(), items.end(),
                [id](const RoadView& item) { return item.id == id; });
            if (found == items.end()) return;
            page.fields[0]->setText(QString::number(found->id));
            page.fields[1]->setText(QString::number(found->startId));
            page.fields[2]->setText(QString::number(found->endId));
            page.fields[3]->setText(number(found->time));
            page.fields[4]->setText(number(found->distance));
        }
        page.action = FormAction::Edit;
    }

    void refreshPage(EntityType type) {
        ManagementPage& page = pageFor(type);
        page.table->setRowCount(0);
        if (type == EntityType::Intersection) {
            page.table->setColumnCount(4);
            page.table->setHorizontalHeaderLabels({"ID", "Ten", "X", "Y"});
            for (const IntersectionView& item : routeService.getMapView().intersections) {
                const int row = page.table->rowCount();
                page.table->insertRow(row);
                const QStringList values = {
                    QString::number(item.id), qString(item.name), number(item.x), number(item.y)};
                for (int column = 0; column < values.size(); ++column)
                    page.table->setItem(row, column, new QTableWidgetItem(values[column]));
            }
        } else {
            page.table->setColumnCount(5);
            page.table->setHorizontalHeaderLabels({"ID", "ID dau", "ID cuoi", "Thoi gian", "Khoang cach"});
            for (const RoadView& item : routeService.getMapView().roads) {
                const int row = page.table->rowCount();
                page.table->insertRow(row);
                const QStringList values = {
                    QString::number(item.id), QString::number(item.startId),
                    QString::number(item.endId), number(item.time), number(item.distance)};
                for (int column = 0; column < values.size(); ++column)
                    page.table->setItem(row, column, new QTableWidgetItem(values[column]));
            }
        }
        page.table->resizeColumnsToContents();
    }

    void save(EntityType type) {
        ManagementPage& page = pageFor(type);
        const auto& fields = page.fields;
        int id = 0;
        if (!parsePositiveInt(fields[0]->text(), id)) {
            QMessageBox::warning(this, "Du lieu khong hop le", "ID phai la so nguyen duong.");
            return;
        }

        std::string error;
        bool success = false;
        if (type == EntityType::Intersection) {
            double x = 0.0;
            double y = 0.0;
            if (fields[1]->text().trimmed().isEmpty() ||
                fields[1]->text().contains('|') ||
                !parseFiniteDouble(fields[2]->text(), x) ||
                !parseFiniteDouble(fields[3]->text(), y)) {
                QMessageBox::warning(this, "Du lieu khong hop le",
                                     "Ten va toa do giao lo khong hop le.");
                return;
            }
            const IntersectionView item{id, fields[1]->text().toStdString(), x, y};
            success = page.action == FormAction::Add
                ? routeService.addIntersection(item, error)
                : routeService.updateIntersection(id, item, error);
        } else {
            int startId = 0;
            int endId = 0;
            double time = 0.0;
            double distance = 0.0;
            if (!parsePositiveInt(fields[1]->text(), startId) ||
                !parsePositiveInt(fields[2]->text(), endId) ||
                !parseFiniteDouble(fields[3]->text(), time) ||
                !parseFiniteDouble(fields[4]->text(), distance)) {
                QMessageBox::warning(this, "Du lieu khong hop le",
                                     "Hay kiem tra ID giao lo, thoi gian va khoang cach.");
                return;
            }
            const RoadView item{id, startId, endId, time, distance};
            success = page.action == FormAction::Add
                ? routeService.addRoad(item, error)
                : routeService.updateRoad(id, item, error);
        }

        if (!success) {
            QMessageBox::warning(this, "Khong the luu", qString(error));
            return;
        }
        refreshPage(type);
        if (type == EntityType::Intersection) refreshPage(EntityType::Road);
        clearFields(type);
        page.action = FormAction::Add;
        QMessageBox::information(this, "Thanh cong", "Da cap nhat ban do va luu vao file.");
    }

    void removeSelected(EntityType type) {
        ManagementPage& page = pageFor(type);
        const int row = page.table->currentRow();
        if (row < 0) {
            QMessageBox::information(this, "Chua chon", "Hay chon mot dong trong danh sach.");
            return;
        }
        const int id = page.table->item(row, 0)->text().toInt();
        const QString message = type == EntityType::Intersection
            ? "Xoa giao lo nay cung se xoa cac duong noi voi no. Tiep tuc?"
            : "Ban co chac muon xoa duong nay?";
        if (QMessageBox::question(this, "Xac nhan xoa", message) != QMessageBox::Yes) return;

        std::string error;
        const bool success = type == EntityType::Intersection
            ? routeService.removeIntersection(id, error)
            : routeService.removeRoad(id, error);
        if (!success) {
            QMessageBox::warning(this, "Khong the xoa", qString(error));
            return;
        }
        refreshPage(type);
        if (type == EntityType::Intersection) refreshPage(EntityType::Road);
        clearFields(type);
        page.action = FormAction::Add;
        QMessageBox::information(this, "Thanh cong", "Da cap nhat ban do va luu vao file.");
    }

    RouteService& routeService;
    ManagementPage intersectionPage;
    ManagementPage roadPage;
};

class MainWindow final : public QMainWindow {
public:
    explicit MainWindow(RouteService& routeService)
        : routeService(routeService), mapView(routeService.getMapView()) {
        setWindowTitle("PBL2 | Ban do va tim duong Dijkstra");
        resize(1360, 850);
        setMinimumSize(1000, 700);
        buildUi();

        const int count = static_cast<int>(mapView.intersections.size());
        taxiId = mapView.intersections.front().id;
        pickupId = mapView.intersections[std::min(7, count - 1)].id;
        destinationId = mapView.intersections[std::min(14, count - 1)].id;
        refreshIntersectionCombos();
        mapWidget->setSelectionIds(taxiId, pickupId, destinationId);
        updateSelectionLabels();

        mapWidget->intersectionClicked = [this](int id) { selectIntersection(id); };
        animationTimer.setInterval(16);
        connect(&animationTimer, &QTimer::timeout, this, [this]() { updateAnimation(); });
    }

private:
    void buildUi() {
        auto* central = new QWidget(this);
        auto* root = new QVBoxLayout(central);
        root->setContentsMargins(22, 18, 22, 18);
        root->setSpacing(14);

        auto* heading = new QVBoxLayout;
        auto* title = new QLabel("PBL2 - MO PHONG BAN DO GIAO THONG", central);
        title->setObjectName("pageTitle");
        auto* subtitle = new QLabel("Qt Widgets + OOP C++ + thuat toan Dijkstra", central);
        subtitle->setObjectName("subtitle");
        heading->addWidget(title);
        heading->addWidget(subtitle);
        root->addLayout(heading);

        auto* content = new QHBoxLayout;
        content->setSpacing(16);
        mapWidget = new MapWidget(mapView, central);
        content->addWidget(mapWidget, 3);
        auto* controls = new QScrollArea(central);
        controls->setWidgetResizable(true);
        controls->setFrameShape(QFrame::NoFrame);
        controls->setWidget(createControlPanel(controls));
        content->addWidget(controls, 2);
        root->addLayout(content, 1);
        setCentralWidget(central);
        setStyleSheet(
            "QMainWindow, QDialog, QWidget { background: #0f172a; color: #f8fafc; }"
            "QGroupBox { background: #19243a; border: 1px solid #334155; border-radius: 12px;"
            " margin-top: 12px; padding: 14px; font-weight: 600; }"
            "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 5px; }"
            "QLabel#pageTitle { font-size: 23px; font-weight: 700; }"
            "QLabel#subtitle, QLabel#hint { color: #a4b2c7; }"
            "QComboBox, QLineEdit, QTableWidget, QTabWidget::pane { background: #111c30;"
            " border: 1px solid #475569; border-radius: 6px; padding: 7px; }"
            "QComboBox QAbstractItemView { background: #19243a; selection-background-color: #2563eb; }"
            "QPushButton { background: #334155; border: 1px solid #475569; border-radius: 7px;"
            " padding: 9px 12px; font-weight: 600; }"
            "QPushButton:hover { background: #475569; }"
            "QPushButton#primaryButton { background: #2563eb; border-color: #3b82f6; }"
            "QPushButton#primaryButton:hover { background: #1d4ed8; }"
            "QPushButton#dangerButton { background: #7f1d1d; border-color: #ef4444; }"
            "QPushButton#secondaryButton { background: #334155; }"
            "QTableWidget { gridline-color: #334155; }"
            "QHeaderView::section { background: #19243a; padding: 6px; border: 0; }"
            "QTabBar::tab { background: #19243a; padding: 9px 18px; }"
            "QTabBar::tab:selected { background: #2563eb; }");
    }

    QWidget* createControlPanel(QWidget* parent) {
        auto* panel = new QWidget(parent);
        auto* layout = new QVBoxLayout(panel);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(12);

        auto* tripGroup = new QGroupBox("DIEU PHOI CHUYEN TAXI", panel);
        auto* tripLayout = new QVBoxLayout(tripGroup);
        auto* hint = new QLabel("Chon vai tro, sau do chon giao lo tren ban do.", tripGroup);
        hint->setObjectName("hint");
        hint->setWordWrap(true);
        tripLayout->addWidget(hint);

        taxiCombo = addSelector(tripLayout, "Vi tri taxi");
        pickupCombo = addSelector(tripLayout, "Diem don");
        destinationCombo = addSelector(tripLayout, "Diem den");
        auto* selectTaxiButton = new QPushButton("Chon taxi tren ban do", tripGroup);
        auto* selectPickupButton = new QPushButton("Chon diem don tren ban do", tripGroup);
        auto* selectDestinationButton = new QPushButton("Chon diem den tren ban do", tripGroup);
        tripLayout->addWidget(selectTaxiButton);
        tripLayout->addWidget(selectPickupButton);
        tripLayout->addWidget(selectDestinationButton);
        layout->addWidget(tripGroup);

        auto* routeGroup = new QGroupBox("TIM DUONG", panel);
        auto* routeLayout = new QVBoxLayout(routeGroup);
        auto* routeButtons = new QHBoxLayout;
        auto* shortestButton = new QPushButton("Duong ngan nhat", routeGroup);
        auto* fastestButton = new QPushButton("Duong nhanh nhat", routeGroup);
        shortestButton->setObjectName("primaryButton");
        fastestButton->setObjectName("primaryButton");
        routeButtons->addWidget(shortestButton);
        routeButtons->addWidget(fastestButton);
        routeLayout->addLayout(routeButtons);
        auto* simulateButton = new QPushButton("Chay mo phong taxi", routeGroup);
        simulateButton->setObjectName("primaryButton");
        auto* resetButton = new QPushButton("Dat lai ket qua", routeGroup);
        auto* manageButton = new QPushButton("Quan ly du lieu", routeGroup);
        routeLayout->addWidget(simulateButton);
        routeLayout->addWidget(resetButton);
        routeLayout->addWidget(manageButton);
        layout->addWidget(routeGroup);

        auto* resultGroup = new QGroupBox("KET QUA CHUYEN DI", panel);
        auto* resultLayout = new QVBoxLayout(resultGroup);
        selectionLabel = new QLabel(resultGroup);
        selectionLabel->setWordWrap(true);
        statusLabel = new QLabel("Chon taxi, diem don va dich tren ban do.", resultGroup);
        statusLabel->setWordWrap(true);
        statusLabel->setStyleSheet("color: #22c55e; font-weight: 600;");
        metricsLabel = new QLabel("Chua co ket qua.", resultGroup);
        metricsLabel->setWordWrap(true);
        pathLabel = new QLabel(resultGroup);
        pathLabel->setWordWrap(true);
        pathLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
        resultLayout->addWidget(selectionLabel);
        resultLayout->addWidget(statusLabel);
        resultLayout->addWidget(metricsLabel);
        resultLayout->addWidget(pathLabel);
        layout->addWidget(resultGroup);

        auto* legend = new QLabel(
            "<span style='color:#22c55e'>● Taxi</span> &nbsp; "
            "<span style='color:#f472b6'>● Diem don</span><br>"
            "<span style='color:#ef4444'>● Diem den</span> &nbsp; "
            "<span style='color:#3b82f6'>● Chang den don</span><br>"
            "<span style='color:#facc15'>● Chang cho khach</span>",
            panel);
        layout->addWidget(legend);
        layout->addStretch();

        auto setTarget = [this](int target) {
            mapWidget->selectionTarget = target;
            statusLabel->setText("Da chon vai tro. Bam vao giao lo tren ban do.");
        };
        connect(selectTaxiButton, &QPushButton::clicked, this, [setTarget]() { setTarget(0); });
        connect(selectPickupButton, &QPushButton::clicked, this, [setTarget]() { setTarget(1); });
        connect(selectDestinationButton, &QPushButton::clicked, this, [setTarget]() { setTarget(2); });
        connect(taxiCombo, qOverload<int>(&QComboBox::currentIndexChanged), this,
                [this]() { syncSelections(); });
        connect(pickupCombo, qOverload<int>(&QComboBox::currentIndexChanged), this,
                [this]() { syncSelections(); });
        connect(destinationCombo, qOverload<int>(&QComboBox::currentIndexChanged), this,
                [this]() { syncSelections(); });
        connect(shortestButton, &QPushButton::clicked, this,
                [this]() { calculateRoute(RouteMode::Shortest); });
        connect(fastestButton, &QPushButton::clicked, this,
                [this]() { calculateRoute(RouteMode::Fastest); });
        connect(simulateButton, &QPushButton::clicked, this, [this]() { startSimulation(); });
        connect(resetButton, &QPushButton::clicked, this, [this]() { resetRoute(); });
        connect(manageButton, &QPushButton::clicked, this, [this]() { manageData(); });
        return panel;
    }

    QComboBox* addSelector(QVBoxLayout* layout, const QString& label) {
        auto* caption = new QLabel(label, this);
        auto* combo = new QComboBox(this);
        layout->addWidget(caption);
        layout->addWidget(combo);
        return combo;
    }

    void refreshIntersectionCombos() {
        const std::vector<IntersectionView>& intersections = mapView.intersections;
        const int oldTaxi = taxiId;
        const int oldPickup = pickupId;
        const int oldDestination = destinationId;
        auto populate = [&intersections](QComboBox* combo, int preferredId) {
            combo->blockSignals(true);
            combo->clear();
            int selectedIndex = 0;
            for (size_t i = 0; i < intersections.size(); ++i) {
                const IntersectionView& item = intersections[i];
                QString label = QString::number(item.id);
                if (!item.name.empty()) label += " - " + qString(item.name);
                combo->addItem(label, item.id);
                if (item.id == preferredId) selectedIndex = static_cast<int>(i);
            }
            if (!intersections.empty()) combo->setCurrentIndex(selectedIndex);
            combo->blockSignals(false);
        };
        populate(taxiCombo, oldTaxi);
        populate(pickupCombo, oldPickup);
        populate(destinationCombo, oldDestination);
        taxiId = currentId(taxiCombo);
        pickupId = currentId(pickupCombo);
        destinationId = currentId(destinationCombo);
        mapWidget->setSelectionIds(taxiId, pickupId, destinationId);
    }

    static int currentId(const QComboBox* combo) {
        return combo->currentData().toInt();
    }

    void syncSelections() {
        animationTimer.stop();
        mapWidget->taxiRunning = false;
        taxiId = currentId(taxiCombo);
        pickupId = currentId(pickupCombo);
        destinationId = currentId(destinationCombo);
        mapWidget->setSelectionIds(taxiId, pickupId, destinationId);
        routeResult = RouteResult{};
        mapWidget->clearRoute();
        metricsLabel->setText("Chua co ket qua.");
        pathLabel->clear();
        updateSelectionLabels();
        statusLabel->setText("Da thay doi giao lo. Hay tim duong lai.");
    }

    void updateSelectionLabels() {
        auto nameFor = [this](int id) {
            const auto& items = mapView.intersections;
            const auto found = std::find_if(items.begin(), items.end(),
                [id](const IntersectionView& item) { return item.id == id; });
            return found == items.end()
                ? QString::number(id)
                : QString("%1 - %2").arg(found->id).arg(qString(found->name));
        };
        selectionLabel->setText(
            "Taxi: " + nameFor(taxiId) + "\nDiem don: " + nameFor(pickupId) +
            "\nDiem den: " + nameFor(destinationId));
    }

    void selectIntersection(int id) {
        QComboBox* target = mapWidget->selectionTarget == 0 ? taxiCombo
            : mapWidget->selectionTarget == 1 ? pickupCombo
                                             : destinationCombo;
        const int index = target->findData(id);
        if (index >= 0) target->setCurrentIndex(index);
    }

    void calculateRoute(RouteMode mode) {
        routeResult = routeService.findTaxiRoute(taxiId, pickupId, destinationId, mode);
        mapWidget->taxiRunning = false;
        animationTimer.stop();
        if (!routeResult.success) {
            mapWidget->clearRoute();
            statusLabel->setText(qString(routeResult.error));
            metricsLabel->setText("Khong co thong tin lo trinh.");
            pathLabel->clear();
            return;
        }

        mapWidget->setRoute(routeResult.intersectionIds, routeResult.roadIds,
                            routeResult.firstLegEdgeCount);
        statusLabel->setText(mode == RouteMode::Fastest
            ? "Da tim hai chang nhanh nhat."
            : "Da tim hai chang ngan nhat.");
        metricsLabel->setText(
            QString("Taxi -> don: %1 km | %2 phut\n"
                    "Don -> dich: %3 km | %4 phut\n"
                    "Tong: %5 km | %6 phut")
                .arg(number(routeResult.firstLegDistance))
                .arg(number(routeResult.firstLegTime))
                .arg(number(routeResult.secondLegDistance))
                .arg(number(routeResult.secondLegTime))
                .arg(number(routeResult.totalDistance))
                .arg(number(routeResult.totalTime)));
        QStringList path;
        for (int id : routeResult.intersectionIds) path.push_back(QString::number(id));
        pathLabel->setText("Lo trinh: " + path.join(" -> "));
    }

    void startSimulation() {
        if (!routeResult.success || routeResult.intersectionIds.size() < 2) {
            statusLabel->setText("Hay tim duong truoc khi mo phong.");
            return;
        }
        mapWidget->taxiRunning = true;
        animationTimer.start();
        taxiTimer.start();
        statusLabel->setText(routeResult.firstLegEdgeCount > 0
            ? "Taxi dang den diem don khach."
            : "Taxi dang cho khach.");
    }

    void updateAnimation() {
        const double duration = std::max(
            1.0, static_cast<double>(routeResult.intersectionIds.size() - 1) * 1.1);
        const double progress = taxiTimer.elapsed() / (duration * 1000.0);
        mapWidget->setAnimationProgress(progress);
        if (progress >= 1.0) {
            animationTimer.stop();
            mapWidget->taxiRunning = false;
            mapWidget->setAnimationProgress(1.0);
            statusLabel->setText("Taxi da dua khach den noi.");
        } else {
            const double edgePosition =
                progress * static_cast<double>(routeResult.intersectionIds.size() - 1);
            statusLabel->setText(edgePosition < routeResult.firstLegEdgeCount
                ? "Taxi dang den diem don khach."
                : "Taxi dang cho khach.");
        }
    }

    void resetRoute() {
        animationTimer.stop();
        mapWidget->taxiRunning = false;
        mapWidget->clearRoute();
        routeResult = RouteResult{};
        metricsLabel->setText("Chua co ket qua.");
        pathLabel->clear();
        statusLabel->setText("Da xoa ket qua tim duong.");
    }

    void manageData() {
        animationTimer.stop();
        mapWidget->taxiRunning = false;
        ManagementDialog dialog(routeService, this);
        dialog.exec();
        routeResult = RouteResult{};
        mapWidget->clearRoute();
        refreshIntersectionCombos();
        updateSelectionLabels();
        metricsLabel->setText("Chua co ket qua.");
        pathLabel->clear();
        statusLabel->setText("Du lieu ban do da duoc cap nhat.");
    }

    RouteService& routeService;
    const MapView& mapView;
    MapWidget* mapWidget = nullptr;
    QComboBox* taxiCombo = nullptr;
    QComboBox* pickupCombo = nullptr;
    QComboBox* destinationCombo = nullptr;
    QLabel* selectionLabel = nullptr;
    QLabel* statusLabel = nullptr;
    QLabel* metricsLabel = nullptr;
    QLabel* pathLabel = nullptr;
    QTimer animationTimer;
    QElapsedTimer taxiTimer;
    RouteResult routeResult;
    int taxiId = -1;
    int pickupId = -1;
    int destinationId = -1;
};
}

int main(int argc, char* argv[]) {
    QApplication application(argc, argv);
    try {
        TextMapRepository repository;
        RouteService routeService(repository);
        if (!routeService.isReady()) {
            QMessageBox::critical(nullptr, "Loi du lieu ban do",
                                  qString(routeService.getLoadError()));
            return 1;
        }
        if (routeService.getMapView().intersections.empty()) {
            QMessageBox::critical(nullptr, "Loi du lieu ban do",
                                  "Ban do khong co giao lo nao.");
            return 1;
        }
        MainWindow window(routeService);
        window.show();
        return application.exec();
    } catch (const std::exception& error) {
        QMessageBox::critical(nullptr, "Loi chuong trinh", QString::fromUtf8(error.what()));
    } catch (...) {
        QMessageBox::critical(nullptr, "Loi chuong trinh", "Loi khong xac dinh.");
    }
    return 1;
}
