#include "MainWindow.hpp"

#include <QLabel>
#include <QWidget>
#include <QTimer>
#include <QPushButton>

#include <QLayout>
#include <QGroupBox>
#include <QFormLayout>

#include <marble/GeoDataPlacemark.h>
#include <marble/GeoDataStyle.h>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    init();
}

void MainWindow::init()
{
    create_gui();
    init_gui();
    create_timer();
    create_connections();
    create_gps();
    create_model();
}

void MainWindow::init_gui()
{
    setWindowTitle( QStringLiteral( "TelemetryHub" ) );
    resize( 800, 600 );


    m_map->setProjection(Marble::Mercator);
    m_map->setMapThemeId("earth/openstreetmap/openstreetmap.dgml");
    //m_map->centerOn( 59.9386, 30.3141 );
}

void MainWindow::create_gui()
{
    m_lbl_lat = new QLabel( "NaN" );
    m_lbl_lon = new QLabel( "NaN" );
    m_btn_set_pos = new QPushButton( "установить позицицю" );

    m_map = new Marble::MarbleWidget( this );

    auto wgt = new QWidget( this );

        auto lo = new QGridLayout( wgt  );
            lo->setContentsMargins ( 0, 0, 0, 0 );
            lo->setSpacing ( 3 );

            auto gbx_telemetry = new QGroupBox ( "Телеметрия" );
                gbx_telemetry->setStyleSheet( "QGroupBox { font-size: 14pt; font-weight: bold; }" );
                gbx_telemetry->setAlignment ( Qt::AlignHCenter );

                auto lo_telemetry = new QFormLayout ( gbx_telemetry );
                    lo_telemetry->addRow( "Широта:" , m_lbl_lat );
                    lo_telemetry->addRow( "Долгота:" , m_lbl_lon );
                    lo_telemetry->addRow( "", m_btn_set_pos );

                auto gbx_map = new QGroupBox ( "Карта"  );
                    gbx_map->setStyleSheet( "QGroupBox { font-size: 14pt; font-weight: bold; }" );
                    gbx_map->setAlignment ( Qt::AlignHCenter );

                    auto lo_map = new QHBoxLayout( gbx_map );
                        lo_map->setContentsMargins ( 0, 0, 0, 0 );
                        lo_map->setSpacing ( 3 );

                        lo_map->addWidget( m_map );


            lo->addWidget( gbx_telemetry, 0, 0 );
            lo->addWidget( gbx_map, 1 , 0 );

            // lo->addWidget( m_lbl_lat, 1, 2 );
            // lo->addWidget( m_lbl_lon, 2, 2 );

            // lo->addWidget( new QLabel( "Широта:" ), 1, 1 );
            // lo->addWidget( new QLabel( "Долгота:" ), 2, 1 );

    setCentralWidget( wgt );
}

void MainWindow::create_connections()
{
    connect( m_timer, &QTimer::timeout, this, [ this ](){

        if( !m_gpsmm )
            return;

        if( !m_gpsmm->waiting( 0 ) )
            return;

        gps_data_t *data = m_gpsmm->read();

        if( data == nullptr )
        {
            //emit errorOccurred( QStringLiteral( "Ошибка чтения данных из gpsd" ) );
            return;
        }

        if( data->fix.mode >= MODE_2D )
        {
            m_lat = data->fix.latitude;
            m_lon = data->fix.longitude;

            update_telemetry();
            add_marker( m_lat, m_lon, "Точка", 3200 );
        }

        //emit satellitesUpdated( data->satellites_used, data->satellites_visible );
    } );

    connect( m_btn_set_pos, &QPushButton::clicked, this, [ this ]() {

        add_marker( m_lat, m_lon, "Точка", 3200 );
    });
}

void MainWindow::create_timer()
{
    m_timer = new QTimer( this );

    m_timer->setInterval( 500 );
}

void MainWindow::create_gps()
{
    start_gps();
}

void MainWindow::create_model()
{
    m_markers_document = new Marble::GeoDataDocument();
    m_markers_document->setName("Мои метки");
}

void MainWindow::update_telemetry()
{
    m_lbl_lat->setText( QString( "%1" ).arg( m_lat, 0, 'f', 6 ) );
    m_lbl_lon->setText( QString( "%1" ).arg( m_lon, 0, 'f', 6 ) );
}

bool MainWindow::start_gps()
{
    stom_gps();

    m_gpsmm.reset( new gpsmm( host.toUtf8().constData(),
                          port.toUtf8().constData() ) );

    if( !m_gpsmm )
    {
        //emit errorOccurred( QStringLiteral( "Не удалось создать gpsmm" ) );
        return false;
    }

    if( m_gpsmm->stream( WATCH_ENABLE | WATCH_JSON ) == nullptr )
    {
        //emit errorOccurred( QStringLiteral( "Не удалось запустить GPS stream" ) );
        m_gpsmm.reset();
        return false;
    }

    m_timer->start();

    return true;
}

void MainWindow::stom_gps()
{
    m_timer->stop();

    if( m_gpsmm )
    {
        m_gpsmm->stream( WATCH_DISABLE );
        m_gpsmm.reset();
    }
}

void MainWindow::add_marker( double lat, double lon, const QString &name, int zoom_level )
{
    Marble::GeoDataPlacemark *placemark = new Marble::GeoDataPlacemark(name);

    // Важно: порядок — долгота, широта!
    placemark->setCoordinate(Marble::GeoDataCoordinates(lon, lat, 0.0,
                                                        Marble::GeoDataCoordinates::Degree));

    // // (по желанию) красивая иконка — можно позже добавить ресурс
    // Marble::GeoDataStyle::Ptr style(new Marble::GeoDataStyle());
    // style->iconStyle().setIconPath(":/icons/pin.png");
    // placemark->setStyle(style);

    m_markers_document->append( placemark );   // ← метка сразу появляется!

    // Центрируем карту на новую метку (по желанию)
    m_map->centerOn(lon, lat);
    m_map->setZoom( zoom_level );
}
