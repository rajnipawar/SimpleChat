#include <gtest/gtest.h>
#include "../src/message.h"

// Simple unit tests that actually work
class SimpleTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup test fixtures
    }
};

// Test message creation and validation
TEST_F(SimpleTest, CreateValidMessage) {
    Message msg("Hello World", "Node1", "Node2", 1);
    
    EXPECT_EQ(msg.getOrigin(), "Node1");
    EXPECT_EQ(msg.getDestination(), "Node2");
    EXPECT_EQ(msg.getChatText(), "Hello World");
    EXPECT_EQ(msg.getSequenceNumber(), 1);
    EXPECT_TRUE(msg.isValid());
}

// Test message with empty content
TEST_F(SimpleTest, EmptyMessageContent) {
    Message msg("", "Node1", "Node2", 1);
    
    EXPECT_FALSE(msg.isValid()); // Empty content should be invalid
    EXPECT_EQ(msg.getChatText(), "");
}

// Test message with zero sequence number  
TEST_F(SimpleTest, ZeroSequenceNumber) {
    Message msg("Hello", "Node1", "Node2", 0);
    
    EXPECT_FALSE(msg.isValid()); // Zero sequence should be invalid (must start from 1)
}

// Test message serialization
TEST_F(SimpleTest, MessageSerialization) {
    Message msg("Test Message", "Node1", "Node2", 5);
    QVariantMap serialized = msg.toVariantMap();
    
    EXPECT_EQ(serialized["Origin"].toString(), "Node1");
    EXPECT_EQ(serialized["Destination"].toString(), "Node2");
    EXPECT_EQ(serialized["ChatText"].toString(), "Test Message");
    EXPECT_EQ(serialized["SequenceNumber"].toInt(), 5);
}

// Test message deserialization
TEST_F(SimpleTest, MessageDeserialization) {
    QVariantMap data;
    data["Origin"] = "Node3";
    data["Destination"] = "Node4";
    data["ChatText"] = "Deserialized Message";
    data["SequenceNumber"] = 10;
    
    Message msg = Message::fromVariantMap(data);
    
    EXPECT_TRUE(msg.isValid());
    EXPECT_EQ(msg.getOrigin(), "Node3");
    EXPECT_EQ(msg.getDestination(), "Node4");
    EXPECT_EQ(msg.getChatText(), "Deserialized Message");
    EXPECT_EQ(msg.getSequenceNumber(), 10);
}

// Test invalid origin/destination
TEST_F(SimpleTest, InvalidNodeIds) {
    Message msg1("Hello", "", "Node2", 1);
    EXPECT_FALSE(msg1.isValid());
    
    Message msg2("Hello", "Node1", "", 1);
    EXPECT_FALSE(msg2.isValid());
    
    Message msg3("Hello", "", "", 1);
    EXPECT_FALSE(msg3.isValid());
}

// Test very long message
TEST_F(SimpleTest, LongMessage) {
    QString longText(1000, 'A'); // 1,000 character message
    Message msg(longText, "Node1", "Node2", 1);
    
    EXPECT_TRUE(msg.isValid());
    EXPECT_EQ(msg.getChatText().length(), 1000);
}

// Test special characters in message
TEST_F(SimpleTest, SpecialCharactersInMessage) {
    QString specialText = "Hello! @#$%^&*()_+ Unicode: 你好 🎉";
    Message msg(specialText, "Node1", "Node2", 1);
    
    EXPECT_TRUE(msg.isValid());
    EXPECT_EQ(msg.getChatText(), specialText);
    
    // Test serialization/deserialization with special chars
    QVariantMap serialized = msg.toVariantMap();
    Message deserialized = Message::fromVariantMap(serialized);
    EXPECT_EQ(deserialized.getChatText(), specialText);
}

// Test multi-line message
TEST_F(SimpleTest, MultiLineMessage) {
    QString multiLineText = "Line 1\nLine 2\nLine 3";
    Message msg(multiLineText, "Node1", "Node2", 1);
    
    EXPECT_TRUE(msg.isValid());
    EXPECT_EQ(msg.getChatText(), multiLineText);
    EXPECT_TRUE(msg.getChatText().contains('\n'));
}

// Test negative sequence number
TEST_F(SimpleTest, NegativeSequenceNumber) {
    Message msg("Hello", "Node1", "Node2", -1);
    
    EXPECT_FALSE(msg.isValid()); // Negative sequence should be invalid
}

// Test message setters
TEST_F(SimpleTest, MessageSetters) {
    Message msg;
    
    msg.setChatText("New Text");
    msg.setOrigin("Origin1");
    msg.setDestination("Dest1");
    msg.setSequenceNumber(42);
    
    EXPECT_EQ(msg.getChatText(), "New Text");
    EXPECT_EQ(msg.getOrigin(), "Origin1");
    EXPECT_EQ(msg.getDestination(), "Dest1");
    EXPECT_EQ(msg.getSequenceNumber(), 42);
    EXPECT_TRUE(msg.isValid());
}

// Test default constructor
TEST_F(SimpleTest, DefaultConstructor) {
    Message msg;
    
    EXPECT_EQ(msg.getChatText(), "");
    EXPECT_EQ(msg.getOrigin(), "");
    EXPECT_EQ(msg.getDestination(), "");
    EXPECT_EQ(msg.getSequenceNumber(), 0);
    EXPECT_FALSE(msg.isValid()); // Default should be invalid (sequence must be >= 1)
}

// Test edge case: very large sequence number
TEST_F(SimpleTest, LargeSequenceNumber) {
    Message msg("Hello", "Node1", "Node2", 999999);
    
    EXPECT_TRUE(msg.isValid());
    EXPECT_EQ(msg.getSequenceNumber(), 999999);
}

// Test serialization round trip
TEST_F(SimpleTest, SerializationRoundTrip) {
    Message original("Round trip test", "OrigNode", "DestNode", 123);
    
    // Serialize and deserialize
    QVariantMap serialized = original.toVariantMap();
    Message roundTrip = Message::fromVariantMap(serialized);
    
    // Should be identical
    EXPECT_EQ(original.getChatText(), roundTrip.getChatText());
    EXPECT_EQ(original.getOrigin(), roundTrip.getOrigin());
    EXPECT_EQ(original.getDestination(), roundTrip.getDestination());
    EXPECT_EQ(original.getSequenceNumber(), roundTrip.getSequenceNumber());
    EXPECT_EQ(original.isValid(), roundTrip.isValid());
}

// Test whitespace-only content
TEST_F(SimpleTest, WhitespaceOnlyContent) {
    Message msg("   ", "Node1", "Node2", 1);
    
    EXPECT_TRUE(msg.isValid()); // Whitespace should be valid content
    EXPECT_EQ(msg.getChatText(), "   ");
}

// Test empty variant map deserialization
TEST_F(SimpleTest, EmptyVariantMapDeserialization) {
    QVariantMap emptyMap;
    Message msg = Message::fromVariantMap(emptyMap);
    
    EXPECT_FALSE(msg.isValid()); // Invalid due to empty fields and sequence number 0
    EXPECT_EQ(msg.getChatText(), "");
    EXPECT_EQ(msg.getOrigin(), "");
    EXPECT_EQ(msg.getDestination(), "");
    EXPECT_EQ(msg.getSequenceNumber(), 0);
}

// Test sequence number starting from 1
TEST_F(SimpleTest, SequenceNumberStartsFromOne) {
    Message msg1("Hello", "Node1", "Node2", 1);
    EXPECT_TRUE(msg1.isValid());
    
    Message msg2("World", "Node1", "Node2", 2);
    EXPECT_TRUE(msg2.isValid());
    
    // Sequence numbers must be positive
    Message invalidMsg("Invalid", "Node1", "Node2", 0);
    EXPECT_FALSE(invalidMsg.isValid());
}

// Basic pass test
TEST_F(SimpleTest, BasicPass) {
    EXPECT_TRUE(true);
    EXPECT_EQ(2 + 2, 4);
    EXPECT_NE(1, 2);
}