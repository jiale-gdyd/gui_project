#include <string>
#include <hope/hope.hpp>

#include "private.h"

using Product = std::string;

struct Producer : public hope::Object {
    Producer() {
        m_timer.set_duration(std::chrono::seconds(1));
        m_timer.triggered().connect(this, &Producer::on_triggered);
    }

    hope::Signal<> &finished() {
        return m_finished_signal;
    }

    hope::Signal<Product> &product_available() {
        return m_product_available_signal;
    }

    void produce() {
        m_timer.start();
    }

private:
    void on_triggered() {
        if (num_products != 5) {
            auto product = "Product " + std::to_string(num_products++);
            std::cout << "Producing product " << product << " from thread " << std::this_thread::get_id() << std::endl;

            m_product_available_signal.emit(std::move(product));
            m_timer.start();
        } else {
            m_finished_signal.emit();
        }
    }

    hope::Timer           m_timer;
    hope::Signal<Product> m_product_available_signal;
    hope::Signal<>        m_finished_signal;
    int                   num_products = 0;
};

struct Consumer : public hope::Object {
    void consume(Product product) {
        std::cout << "Consuming " << product << " from thread " << std::this_thread::get_id() << std::endl;
    }
};

int host_unittest_sigslot_threaded_producer_consumer_exit(void)
{
    return -1;
}

int host_unittest_sigslot_threaded_producer_consumer_init(int argc, char *argv[])
{
    hope::Application app;
    std::unique_ptr<Consumer> consumer (new Consumer());

    Producer producer;
    producer.product_available().connect(consumer.get(), &Consumer::consume);
    producer.finished().connect(&app, &hope::Application::quit);

    hope::Thread consumer_thread;
    consumer_thread.start();
    consumer_thread.move_to_thread(std::move(consumer));

    producer.produce();

    return app.exec();
}
