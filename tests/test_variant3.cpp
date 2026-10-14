#include <gtest/gtest.h>
#include <sstream>
#include <string>

#include "variant3.h"

// ---------- Успешные сценарии: добавление ----------

TEST(AddCommandTest, AddPencilSuccess) {
    Container items;
    std::string line = "ADD PENCIL 2 BLUE 50.5 +79991234567";
    applyCommand(line, items);

    ASSERT_EQ(items.size(), 1u);
    EXPECT_EQ(items[0]->getType(), "PENCIL");
    EXPECT_DOUBLE_EQ(items[0]->getPrice(), 50.5);
    EXPECT_EQ(items[0]->getPhone(), "+79991234567");
}

TEST(AddCommandTest, AddPenSuccess) {
    Container items;
    applyCommand("ADD PEN BALL 0.7 80 +79991112233", items);

    ASSERT_EQ(items.size(), 1u);
    EXPECT_EQ(items[0]->getType(), "PEN");
    EXPECT_DOUBLE_EQ(items[0]->getPrice(), 80.0);
    EXPECT_EQ(items[0]->getPhone(), "+79991112233");
}

TEST(AddCommandTest, AddPaperSuccess) {
    Container items;
    applyCommand("ADD PAPER 80 210 297 35 +79990001122", items);

    ASSERT_EQ(items.size(), 1u);
    EXPECT_EQ(items[0]->getType(), "PAPER");
    EXPECT_DOUBLE_EQ(items[0]->getPrice(), 35.0);
    EXPECT_EQ(items[0]->getPhone(), "+79990001122");
}

// ---------- Успешные сценарии: удаление ----------

TEST(RemoveCommandTest, RemoveByType) {
    Container items;
    applyCommand("ADD PENCIL 2 BLUE 50.5 +79991234567", items);
    applyCommand("ADD PEN BALL 0.7 80 +79991112233", items);
    applyCommand("ADD PAPER 80 210 297 35 +79990001122", items);

    applyCommand("REM TYPE PENCIL", items);

    ASSERT_EQ(items.size(), 2u);
    EXPECT_EQ(items[0]->getType(), "PEN");
    EXPECT_EQ(items[1]->getType(), "PAPER");
}

TEST(RemoveCommandTest, RemoveByPriceGreater) {
    Container items;
    applyCommand("ADD PENCIL 2 BLUE 50.5 +79991234567", items);
    applyCommand("ADD PEN BALL 0.7 80 +79991112233", items);
    applyCommand("ADD PAPER 80 210 297 35 +79990001122", items);

    applyCommand("REM PRICE > 60", items);

    ASSERT_EQ(items.size(), 2u);
    EXPECT_DOUBLE_EQ(items[0]->getPrice(), 50.5);
    EXPECT_DOUBLE_EQ(items[1]->getPrice(), 35.0);
}

TEST(RemoveCommandTest, RemoveByPriceLess) {
    Container items;
    applyCommand("ADD PENCIL 2 BLUE 50.5 +79991234567", items);
    applyCommand("ADD PAPER 80 210 297 35 +79990001122", items);

    applyCommand("REM PRICE < 40", items);

    ASSERT_EQ(items.size(), 1u);
    EXPECT_DOUBLE_EQ(items[0]->getPrice(), 50.5);
}

TEST(RemoveCommandTest, RemoveByPhone) {
    Container items;
    applyCommand("ADD PENCIL 2 BLUE 50.5 +79991234567", items);
    applyCommand("ADD PEN BALL 0.7 80 +79991112233", items);

    applyCommand("REM PHONE +79991234567", items);

    ASSERT_EQ(items.size(), 1u);
    EXPECT_EQ(items[0]->getPhone(), "+79991112233");
}

// ---------- Форматирование ----------

TEST(PrintTest, FormatPencil) {
    Pencil pencil(2, Color::BLUE, 50.5, "+79991234567");
    std::string output = formatItem(pencil);

    EXPECT_NE(output.find("PENCIL"), std::string::npos);
    EXPECT_NE(output.find("leadDensity=2"), std::string::npos);
    EXPECT_NE(output.find("color=BLUE"), std::string::npos);
    EXPECT_NE(output.find("price=50.50"), std::string::npos);
}

TEST(PrintTest, FormatPen) {
    Pen pen("BALL", 0.7, 80.0, "+79991112233");
    std::string output = formatItem(pen);

    EXPECT_NE(output.find("PEN"), std::string::npos);
    EXPECT_NE(output.find("type=BALL"), std::string::npos);
    EXPECT_NE(output.find("diameter=0.70"), std::string::npos);
}

TEST(PrintTest, FormatPaper) {
    Paper paper(80, 210, 297, 35.0, "+79990001122");
    std::string output = formatItem(paper);

    EXPECT_NE(output.find("PAPER"), std::string::npos);
    EXPECT_NE(output.find("density=80"), std::string::npos);
    EXPECT_NE(output.find("width=210"), std::string::npos);
    EXPECT_NE(output.find("height=297"), std::string::npos);
}

// ---------- Исключительные ситуации: недостаток параметров ----------

TEST(AddCommandTest, AddPencilMissingParameters) {
    Container items;
    applyCommand("ADD PENCIL 2 BLUE 50.5", items);
    EXPECT_TRUE(items.empty());
}

TEST(AddCommandTest, AddPenMissingParameters) {
    Container items;
    applyCommand("ADD PEN BALL 0.7", items);
    EXPECT_TRUE(items.empty());
}

TEST(AddCommandTest, AddPaperMissingParameters) {
    Container items;
    applyCommand("ADD PAPER 80 210 297 35", items);
    EXPECT_TRUE(items.empty());
}

// ---------- Исключительные ситуации: неверные значения перечислимых типов ----------

TEST(AddCommandTest, AddPencilInvalidColor) {
    Container items;
    applyCommand("ADD PENCIL 2 PURPLE 50.5 +79991234567", items);
    EXPECT_TRUE(items.empty());
}

TEST(AddCommandTest, AddPenInvalidType) {
    Container items;
    applyCommand("ADD PEN FOUNTAIN 0.7 80 +79991112233", items);
    EXPECT_TRUE(items.empty());
}

// ---------- Исключительные ситуации: неверный числовой формат ----------

TEST(AddCommandTest, AddPencilInvalidPrice) {
    Container items;
    applyCommand("ADD PENCIL 2 BLUE abc +79991234567", items);
    EXPECT_TRUE(items.empty());
}

TEST(AddCommandTest, AddPaperInvalidDimensions) {
    Container items;
    applyCommand("ADD PAPER abc 210 297 35 +79990001122", items);
    EXPECT_TRUE(items.empty());
}

// ---------- Исключительные ситуации: неизвестные команды и поля ----------

TEST(CommandTest, UnknownCommand) {
    Container items;
    applyCommand("DELETE ALL", items);
    EXPECT_TRUE(items.empty());
}

TEST(RemoveCommandTest, UnknownRemoveField) {
    Container items;
    applyCommand("ADD PENCIL 2 BLUE 50.5 +79991234567", items);

    applyCommand("REM COLOR RED", items);

    EXPECT_EQ(items.size(), 1u);
}

// ---------- Граничные случаи ----------

TEST(RemoveCommandTest, RemoveFromEmptyContainer) {
    Container items;
    applyCommand("REM TYPE PENCIL", items);
    EXPECT_TRUE(items.empty());
}

TEST(RemoveCommandTest, RemoveNonexistentType) {
    Container items;
    applyCommand("ADD PENCIL 2 BLUE 50.5 +79991234567", items);

    applyCommand("REM TYPE PEN", items);

    EXPECT_EQ(items.size(), 1u);
}