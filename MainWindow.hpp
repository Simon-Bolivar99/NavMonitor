#pragma once

#include <QMainWindow>

#include <libgpsmm.h>

#include <marble/MarbleWidget.h>
#include <marble/GeoDataDocument.h>
#include <marble/MarbleModel.h>

class QLabel;
class QTimer;
class QPushButton;

static double dnan = std::numeric_limits< double >::quiet_NaN();

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private:
// ------------ [ init ] ------------
    void init();

    void init_gui();
    void create_gui();
    void create_connections();
    void create_timer();
    void create_gps();
    void create_model();

// ------------ [  ] ------------
    void update_telemetry();

    bool start_gps();
    void stom_gps();

    void add_marker( double lat, double lon, const QString &name, int zoom_level );

private:
    QString host = "127.0.0.1";
    QString port = "2947";

    double m_lat = dnan;
    double m_lon = dnan;

    QLabel* m_lbl_lat = nullptr;
    QLabel* m_lbl_lon = nullptr;

    QPushButton* m_btn_set_pos;

    QScopedPointer< gpsmm > m_gpsmm ;

    QTimer *m_timer = nullptr;

    Marble::MarbleWidget *m_map = nullptr;

    Marble::GeoDataDocument *m_markers_document = nullptr;
};
