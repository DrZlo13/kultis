#include <thread>
#include <hal/hal.h>
#include <mutex>
#include <bitset>

#include <QtWidgets>
#include <QImage>

static constexpr size_t DISPLAY_WIDTH = 128;
static constexpr size_t DISPLAY_HEIGHT = 64;
static constexpr size_t DISPLAY_SCALE = 4;
static constexpr size_t DISPLAY_WIDTH_SCALED = DISPLAY_WIDTH * DISPLAY_SCALE;
static constexpr size_t DISPLAY_HEIGHT_SCALED = DISPLAY_HEIGHT * DISPLAY_SCALE;

DisplayBuffer display_buffer_handler;
static std::bitset<DISPLAY_WIDTH * DISPLAY_HEIGHT> display_buffer;
static std::mutex display_buffer_mutex;

void DisplayBuffer::set_pixel(size_t x, size_t y, bool pixel) {
    display_buffer[(y * DISPLAY_WIDTH) + (x)] = pixel;
}

void DisplayBuffer::fill(bool value) {
    if(value) {
        display_buffer.set();
    } else {
        display_buffer.reset();
    }
}

class HALEmulator;

QApplication* main_app;
HALEmulator* hal_emulator;

class DisplayWidget : public QWidget {
private:
    QImage _image;
    static const size_t buffer_colors = 3;
    uchar _buffer[DISPLAY_HEIGHT][DISPLAY_WIDTH][buffer_colors];

    void copy_buffer_to_image() {
        const std::lock_guard<std::mutex> lock(display_buffer_mutex);

        // copy buffer to image
        for(size_t y = 0; y < DISPLAY_HEIGHT; y++) {
            for(size_t x = 0; x < DISPLAY_WIDTH; x++) {
                bool bit = display_buffer[(y * DISPLAY_WIDTH) + (x)];
                _buffer[y][x][0] = bit * 255;
                _buffer[y][x][1] = bit * 255;
                _buffer[y][x][2] = bit * 255;
            }
        }
    }

public:
    explicit DisplayWidget(QWidget* parent = 0) {
        memset(_buffer, 0, DISPLAY_HEIGHT * DISPLAY_WIDTH * buffer_colors);
        _image = QImage((uchar*)_buffer, DISPLAY_WIDTH, DISPLAY_HEIGHT, QImage::Format_RGB888);
    }

    ~DisplayWidget(){

    };

    void force_redraw() {
        copy_buffer_to_image();
        update();
    }

protected:
    void paintEvent(QPaintEvent* event) {
        QPainter painter(this);
        painter.scale(DISPLAY_SCALE, DISPLAY_SCALE);
        painter.drawImage(0, 0, this->_image);
    }
};

class HALEmulator : public QWidget {
private:
    static const size_t buttons_count = 6;
    DisplayWidget* _display;
    QHBoxLayout* mainLayout;
    QGroupBox* inputs;
    QPushButton* button[buttons_count];

private slots:
    void handle_button_pressed() {
        QPushButton* button = (QPushButton*)sender();
        printf("Button %s pressed\r\n", button->text().toStdString().c_str());
    }

    void handle_button_released() {
        QPushButton* button = (QPushButton*)sender();
        printf("Button %s released\r\n", button->text().toStdString().c_str());
    }

public:
    HALEmulator(QWidget* parent = 0) {
        _display = new DisplayWidget(this);
        _display->resize(DISPLAY_WIDTH_SCALED, DISPLAY_HEIGHT_SCALED);
        _display->setFixedSize(DISPLAY_WIDTH_SCALED, DISPLAY_HEIGHT_SCALED);

        inputs = new QGroupBox();
        QGridLayout* layout = new QGridLayout;
        button[0] = new QPushButton(tr("U"));
        layout->addWidget(button[0], 0, 1);
        button[1] = new QPushButton(tr("D"));
        layout->addWidget(button[1], 2, 1);
        button[2] = new QPushButton(tr("L"));
        layout->addWidget(button[2], 1, 0);
        button[3] = new QPushButton(tr("R"));
        layout->addWidget(button[3], 1, 2);
        button[4] = new QPushButton(tr("O"));
        layout->addWidget(button[4], 1, 1);
        button[5] = new QPushButton(tr("<"));
        layout->addWidget(button[5], 2, 2);

        for(size_t i = 0; i < buttons_count; i++) {
            button[i]->setMaximumHeight(50);
            button[i]->setMaximumWidth(50);
            connect(button[i], &QPushButton::pressed, this, &HALEmulator::handle_button_pressed);
            connect(button[i], &QPushButton::released, this, &HALEmulator::handle_button_released);
        }

        inputs->setLayout(layout);

        mainLayout = new QHBoxLayout;
        mainLayout->addWidget(_display);
        mainLayout->addWidget(inputs);
        setLayout(mainLayout);

        setWindowTitle(QApplication::translate("halemulator", "HAL Emulator"));
    }

    ~HALEmulator() {
    }

    void force_display_redraw() {
        _display->force_redraw();
    }
};

HALDisplay::HALDisplay() {
}

HALDisplay::~HALDisplay() {
}

size_t HALDisplay::get_width() {
    return DISPLAY_WIDTH;
}

size_t HALDisplay::get_height() {
    return DISPLAY_HEIGHT;
}

DisplayBuffer* HALDisplay::get_display_buffer() {
    display_buffer_mutex.lock();
    return &display_buffer_handler;
}

void HALDisplay::commit_display_buffer(bool redraw) {
    display_buffer_mutex.unlock();
    if(redraw) {
        hal_emulator->force_display_redraw();
    }
}

void hal_pre_init(void) {
    int argc = 0;
    char** argv = NULL;
    main_app = new QApplication(argc, argv);
    hal_emulator = new HALEmulator();
    hal_emulator->show();
}

void hal_post_init(void) {
    main_app->exec();
}