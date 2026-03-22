#pragma once

#include <QMainWindow>

#include <libgpsmm.h>

//#include <limits.h>

#include <MarbleWidget.h>
#include <GeoDataDocument.h>
#include <MarbleModel.h>
#include <QElapsedTimer>

class QLabel;
class QTimer;
class QPushButton;

static double dnan = std::numeric_limits< double >::quiet_NaN();

struct TPV
{
    TPV() = default;

    double lat = dnan;
    double lon = dnan;
    double alt = dnan;

};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void on_poll_gps();

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
    void stop_gps();

    void add_marker( double lat, double lon, const QString &name, int zoom_level );

private:
    QString host = "127.0.0.1";
    QString port = "2947";

    // double m_lat = dnan;
    // double m_lon = dnan;

    TPV m_tpv;

    QLabel* m_lbl_lat = nullptr;
    QLabel* m_lbl_lon = nullptr;

    QPushButton* m_btn_set_pos;

    QScopedPointer< gpsmm > m_gpsmm ;

    QTimer *m_timer = nullptr;
    QElapsedTimer m_last_data_timer;

   Marble::MarbleWidget *m_map = nullptr;

   Marble::GeoDataDocument *m_markers_document = nullptr;

    bool m_has_received_data = false;
};
