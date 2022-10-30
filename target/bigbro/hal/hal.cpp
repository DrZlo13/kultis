#include <thread>
#include <hal/hal.h>
#include <mutex>
#include <bitset>

#include <QtWidgets>
#include <QImage>
#include <QPlainTextEdit>

typedef struct {
    InputCallback callback;
    void* context;
} InputCallbackRecord;

static constexpr size_t DISPLAY_WIDTH = 128;
static constexpr size_t DISPLAY_HEIGHT = 64;
static constexpr size_t DISPLAY_SCALE = 4;
static constexpr size_t DISPLAY_WIDTH_SCALED = DISPLAY_WIDTH * DISPLAY_SCALE;
static constexpr size_t DISPLAY_HEIGHT_SCALED = DISPLAY_HEIGHT * DISPLAY_SCALE;

DisplayBuffer display_buffer_handler;
static std::bitset<DISPLAY_WIDTH * DISPLAY_HEIGHT> display_buffer;
static std::mutex display_buffer_mutex;

static std::mutex input_callback_mutex;
static std::vector<InputCallbackRecord> input_callbacks;

static const char* button_names[] = {
    [static_cast<uint8_t>(InputKey::Up)] = "U",
    [static_cast<uint8_t>(InputKey::Down)] = "D",
    [static_cast<uint8_t>(InputKey::Right)] = "L",
    [static_cast<uint8_t>(InputKey::Left)] = "R",
    [static_cast<uint8_t>(InputKey::Ok)] = "O",
    [static_cast<uint8_t>(InputKey::Back)] = "<",
};

bool get_key_from_button_name(const char* name, InputKey* key) {
    for(size_t i = 0; i < sizeof(button_names) / sizeof(button_names[0]); i++) {
        if(strcmp(button_names[i], name) == 0) {
            *key = static_cast<InputKey>(i);
            return true;
        }
    }
    return false;
}

void DisplayBuffer::set_pixel(size_t x, size_t y, bool pixel) {
    if(x < DISPLAY_WIDTH && y < DISPLAY_HEIGHT) {
        display_buffer[(y * DISPLAY_WIDTH) + (x)] = pixel;
    }
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

typedef struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;

static Color color_set = {0x00, 0x00, 0x00};
static Color color_reset = {0xFF, 0x82, 0x00};

class DisplayWidget : public QWidget {
private:
    QImage _image;
    static const size_t buffer_colors = 3;
    uchar _buffer[DISPLAY_HEIGHT][DISPLAY_WIDTH][buffer_colors];

    void set_pixel(size_t x, size_t y, bool pixel) {
        if(pixel) {
            _buffer[y][x][0] = color_set.r;
            _buffer[y][x][1] = color_set.g;
            _buffer[y][x][2] = color_set.b;
        } else {
            _buffer[y][x][0] = color_reset.r;
            _buffer[y][x][1] = color_reset.g;
            _buffer[y][x][2] = color_reset.b;
        }
    }

    void copy_buffer_to_image() {
        const std::lock_guard<std::mutex> lock(display_buffer_mutex);

        // copy buffer to image
        for(size_t y = 0; y < DISPLAY_HEIGHT; y++) {
            for(size_t x = 0; x < DISPLAY_WIDTH; x++) {
                bool bit = display_buffer[(y * DISPLAY_WIDTH) + (x)];
                set_pixel(x, y, bit);
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
    QPlainTextEdit* log;

    void send_input_event(QPushButton* button, InputType type) {
        std::string name = button->text().toStdString();

        InputEvent event;
        event.type = type;

        if(get_key_from_button_name(name.c_str(), &event.key)) {
            const std::lock_guard<std::mutex> lock(input_callback_mutex);
            for(auto& callback : input_callbacks) {
                callback.callback(&event, callback.context);
            }
        }
    }

    QFont get_monospace_font() {
        QFont font;
        font.setStyleHint(QFont::Monospace);
        font.setFamily("Monospace");
        font.setFixedPitch(true);
        return font;
    }

private slots:

    void handle_button_pressed() {
        QPushButton* button = (QPushButton*)sender();
        send_input_event(button, InputType::Press);
    }

    void handle_button_released() {
        QPushButton* button = (QPushButton*)sender();
        send_input_event(button, InputType::Release);
    }

public:
    HALEmulator(QWidget* parent = 0) {
        _display = new DisplayWidget(this);
        _display->resize(DISPLAY_WIDTH_SCALED, DISPLAY_HEIGHT_SCALED);
        _display->setFixedSize(DISPLAY_WIDTH_SCALED, DISPLAY_HEIGHT_SCALED);

        inputs = new QGroupBox();
        QGridLayout* layout = new QGridLayout;
        button[0] = new QPushButton(tr(button_names[static_cast<uint8_t>(InputKey::Up)]));
        layout->addWidget(button[0], 0, 1);
        button[1] = new QPushButton(tr(button_names[static_cast<uint8_t>(InputKey::Down)]));
        layout->addWidget(button[1], 2, 1);
        button[2] = new QPushButton(tr(button_names[static_cast<uint8_t>(InputKey::Left)]));
        layout->addWidget(button[2], 1, 0);
        button[3] = new QPushButton(tr(button_names[static_cast<uint8_t>(InputKey::Right)]));
        layout->addWidget(button[3], 1, 2);
        button[4] = new QPushButton(tr(button_names[static_cast<uint8_t>(InputKey::Ok)]));
        layout->addWidget(button[4], 1, 1);
        button[5] = new QPushButton(tr(button_names[static_cast<uint8_t>(InputKey::Back)]));
        layout->addWidget(button[5], 2, 2);

        for(size_t i = 0; i < buttons_count; i++) {
            button[i]->setMaximumHeight(50);
            button[i]->setMaximumWidth(50);
            connect(button[i], &QPushButton::pressed, this, &HALEmulator::handle_button_pressed);
            connect(button[i], &QPushButton::released, this, &HALEmulator::handle_button_released);
        }

        inputs->setLayout(layout);

        log = new QPlainTextEdit();
        log->setFont(get_monospace_font());
        log->setReadOnly(true);

        QHBoxLayout* screen_layout = new QHBoxLayout;
        screen_layout->addWidget(_display);
        screen_layout->addWidget(inputs);

        QVBoxLayout* top_layout = new QVBoxLayout;
        top_layout->addLayout(screen_layout);
        top_layout->addWidget(log);

        mainLayout = new QHBoxLayout;
        mainLayout->addLayout(top_layout);
        setLayout(mainLayout);

        setWindowTitle(QApplication::translate("halemulator", "HAL Emulator"));
        log_message("HAL Emulator started");
    }

    ~HALEmulator() {
    }

    void force_display_redraw() {
        _display->force_redraw();
    }

    void log_message(const char* message) {
        QMetaObject::invokeMethod(
            log, "appendHtml", Qt::QueuedConnection, Q_ARG(QString, QString(message)));
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

/***************************** Input *****************************/

void hal_input_init(void) {
}

void hal_input_set_callback(InputCallback callback, void* context) {
    const std::lock_guard<std::mutex> lock(input_callback_mutex);
    input_callbacks.push_back({callback, context});
}

/***************************** Log *****************************/

#include <string>
#include <chrono>
#include <ctime>

static uint32_t start_time;

void hal_log_init() {
    start_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                     std::chrono::system_clock::now().time_since_epoch())
                     .count();
}

uint32_t log_get_time() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
               .count() -
           start_time;
}

void hal_log_raw(const char* text) {
    hal_emulator->log_message(text);
}

static const char* log_colors[] = {
    [static_cast<uint8_t>(LogLevel::Error)] = "red",
    [static_cast<uint8_t>(LogLevel::Warning)] = "brown",
    [static_cast<uint8_t>(LogLevel::Info)] = "green",
    [static_cast<uint8_t>(LogLevel::Debug)] = "blue",
    [static_cast<uint8_t>(LogLevel::Trace)] = "purple",
};

static const char* log_letters[] = {
    [static_cast<uint8_t>(LogLevel::Error)] = "E",
    [static_cast<uint8_t>(LogLevel::Warning)] = "W",
    [static_cast<uint8_t>(LogLevel::Info)] = "I",
    [static_cast<uint8_t>(LogLevel::Debug)] = "D",
    [static_cast<uint8_t>(LogLevel::Trace)] = "T",
};

void hal_log(LogLevel level, const char* tag, const char* format, ...) {
    const char* color = log_colors[static_cast<uint8_t>(level)];
    const char* letter = log_letters[static_cast<uint8_t>(level)];
    std::string time = std::to_string(log_get_time());
    std::string message =
        time + " <font color=\"" + std::string(color) + "\">[" + letter + "][" + tag + "]</font> ";

    va_list args;
    va_start(args, format);
    char* buffer = nullptr;
    int size = vasprintf(&buffer, format, args);
    va_end(args);
    if(size > 0) {
        message += buffer;
        free(buffer);
    }

    hal_emulator->log_message(message.c_str());
}

/***************************** HAL *****************************/

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