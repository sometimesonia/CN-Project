#include <iostream>

class ApplicationLayer {
public:
    virtual void use() = 0;
};

class HTTP : public ApplicationLayer {
private:
    int id;
    int port;

public:
    HTTP(int id) : id(id), port(80) {}

    void use() override {
        std::cout << "Using HTTP Application" << std::endl;
    }
};

class SSH : public ApplicationLayer {
private:
    int id;
    int port;

public:
    SSH(int id) : id(id), port(22) {}

    void use() override {
        std::cout << "Using SSH Application" << std::endl;
    }
};

int main() {
    ApplicationLayer* httpApp = new HTTP(1);
    httpApp->use();

    ApplicationLayer* sshApp = new SSH(2);
    sshApp->use();

    delete httpApp;
    delete sshApp;

    return 0;
}
