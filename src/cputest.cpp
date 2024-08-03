#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <filesystem>

#include <nlohmann/json.hpp>
#include <gtest/gtest.h>

#define private public
#include "cpu/cpu.h"
#undef private

#include "address_bus.h"

using nlohmann::json;

struct States {
    uint8_t a, b, c, d, e, f, h, l;
    uint16_t pc, sp;
    std::vector<std::pair<uint16_t, uint8_t>> ram;
    States(const json& j) {
        a = j["a"];
        b = j["b"];
        c = j["c"];
        d = j["d"];
        e = j["e"];
        f = j["f"];
        h = j["h"];
        l = j["l"];
        pc = static_cast<uint16_t>(j["pc"]) - 1;
        sp = j["sp"];
        const auto& r = j["ram"];
        std::transform(r.begin(), r.end(), std::back_inserter(ram), [](auto arg) {
            return std::make_pair(arg[0], arg[1]);
        });
    }
};

class CustomMemory : public AddressBus {
public:
    CustomMemory() {
        for (int i = 0; i < (1 << 16); i++) {
            setReader(i, values[i]);
            setWriter(i, [&, i](uint8_t byte) {
                values[i] = byte;
                history.push_back(i);
            });
        }
    }
    void revert() {
        for (auto addr : history) {
            values[addr] = 0;
        }
        history.clear();
    }
private:
    std::array<uint8_t, 1 << 16> values;
    std::vector<uint16_t> history;
};

class CPUTest : public ::testing::Test {
public:
    CPUTest(const std::pair<States, States>& data) : init(data.first), final(data.second) {}
    void SetUp() override {
        c.af.high = init.a;
        c.af.low = init.f;
        c.bc.high = init.b;
        c.bc.low = init.c;
        c.de.high = init.d;
        c.de.low = init.e;
        c.hl.high = init.h;
        c.hl.low = init.l;
        c.pc = init.pc;
        c.sp = init.sp;
        for (const auto& [addr, byte] : init.ram) {
            m.write(addr, byte);
        }
        run(final.pc);
    }
    void TestBody() override {
        EXPECT_EQ(c.af.high, final.a);
        EXPECT_EQ(c.af.low, final.f);
        EXPECT_EQ(c.bc.high, final.b);
        EXPECT_EQ(c.bc.low, final.c);
        EXPECT_EQ(c.de.high, final.d);
        EXPECT_EQ(c.de.low, final.e);
        EXPECT_EQ(c.hl.high, final.h);
        EXPECT_EQ(c.hl.low, final.l);
        EXPECT_EQ(static_cast<uint16_t>(c.pc), final.pc);
        EXPECT_EQ(static_cast<uint16_t>(c.sp), final.sp);
        for (const auto& [addr, byte] : final.ram) {
            EXPECT_EQ(m.read(addr), byte);
        }
    }
    void TearDown() override {
        m.revert();
    }

private:
    void run(int to) {
        while (c.pc != to) {
            c.tick();
        }
    }
    States init, final;
    static CPU c;
    static CustomMemory m;
};

CustomMemory CPUTest::m;
CPU CPUTest::c(m);

void registerTests() {
    namespace fs = std::filesystem;
    for (const auto& f : fs::directory_iterator("tests")) {
        std::string name = f.path().stem().string();
        std::ifstream file(f.path());
        json j;
        file >> j;
        std::for_each(j.begin(), j.end(), [&, testIndex = 0](const auto& arg) mutable {
            testing::RegisterTest(("0x" + name).c_str(), ("Test " + std::to_string(testIndex++)).c_str(), nullptr, arg.dump(1).c_str(), __FILE__, __LINE__, [=]() {
                return new CPUTest(std::make_pair(States(arg["initial"]), States(arg["final"])));
            });
        });
    }
}
int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    registerTests();
    return RUN_ALL_TESTS();
}