#include "entities/item.h"

#include <gtest/gtest.h>

TEST(ItemTest, StoresNameRarityAndBasePrice) {
  Item sword("Iron Sword", ItemRarity::Rare, 100);

  EXPECT_EQ(sword.getName(), "Iron Sword");
  EXPECT_EQ(sword.getRarity(), ItemRarity::Rare);
  EXPECT_EQ(sword.getBasePrice(), 100);
}

TEST(ItemTest, ReturnsCorrectRarityName) {
  Item item("Potion", ItemRarity::Epic, 20);

  EXPECT_EQ(item.getRarityName(), "Epic");
}

TEST(ItemTest, CalculatesSellPriceUsingRarityMultiplier) {
  Item commonItem("Stone", ItemRarity::Common, 10);
  Item rareItem("Crystal", ItemRarity::Rare, 10);
  Item legendaryItem("Sword", ItemRarity::Legendary, 10);

  EXPECT_EQ(commonItem.getSellPrice(), 10);
  EXPECT_EQ(rareItem.getSellPrice(), 20);
  EXPECT_EQ(legendaryItem.getSellPrice(), 50);
}

TEST(ItemTest, NegativeBasePriceBecomesZero) {
  Item brokenItem("Broken Item", ItemRarity::Common, -100);

  EXPECT_EQ(brokenItem.getBasePrice(), 0);
  EXPECT_EQ(brokenItem.getSellPrice(), 0);
}
