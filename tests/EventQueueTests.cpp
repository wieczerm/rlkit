#include "../src/core/Event.hpp"
#include "../src/core/EventQueue.hpp"
#include "assertions.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace core;

// Test basic push and query
void testPushAndQuery() {
  std::cout << "Testing push and query..." << std::endl;

  EventQueue queue;

  EXPECT_TRUE(queue.empty());
  EXPECT_EQ(queue.size(), 0u);

  queue.push(TestEvent{42, "test"});

  EXPECT_FALSE(queue.empty());
  EXPECT_EQ(queue.size(), 1u);

  queue.push(TestEvent{99, "another"});
  EXPECT_EQ(queue.size(), 2u);

  std::cout << "  ✓ Push and query work correctly" << std::endl;
}

// Test clear
void testClear() {
  std::cout << "Testing clear..." << std::endl;

  EventQueue queue;
  queue.push(TestEvent{1, "a"});
  queue.push(TestEvent{2, "b"});
  queue.push(TestEvent{3, "c"});

  EXPECT_EQ(queue.size(), 3u);

  queue.clear();

  EXPECT_TRUE(queue.empty());
  EXPECT_EQ(queue.size(), 0u);

  std::cout << "  ✓ Clear empties the queue" << std::endl;
}

// Test event processing with callback
void testProcess() {
  std::cout << "Testing process with callback..." << std::endl;

  EventQueue queue;
  queue.push(TestEvent{10, "first"});
  queue.push(TestEvent{20, "second"});
  queue.push(TestEvent{30, "third"});

  std::vector<int> processed_values;

  queue.process([&processed_values](const Event &e) {
    if (std::holds_alternative<TestEvent>(e)) {
      const auto &test_evt = std::get<TestEvent>(e);
      processed_values.push_back(test_evt.value);
    }
  });

  // Check all events were processed
  EXPECT_EQ(processed_values.size(), 3u);
  EXPECT_EQ(processed_values[0], 10);
  EXPECT_EQ(processed_values[1], 20);
  EXPECT_EQ(processed_values[2], 30);

  // Queue should be empty after processing
  EXPECT_TRUE(queue.empty());

  std::cout << "  ✓ Process executes callback for all events" << std::endl;
}

// Test priority ordering
void testPriorityOrdering() {
  std::cout << "Testing priority ordering..." << std::endl;

  EventQueue queue;

  // Push events in mixed priority order
  queue.push(EntityMovedEvent{1, {0, 0}, {1, 1}}); // Normal priority
  queue.push(
      EntityDiedEvent{2, "goblin", {5, 5}, -1, "lava"}); // Immediate priority
  queue.push(EntityDamagedEvent{3, 1, 50, 20, "physical"}); // High priority
  queue.push(EntityMovedEvent{4, {2, 2}, {3, 3}});          // Normal priority

  std::vector<std::string> event_types;

  queue.process([&event_types](const Event &e) {
    std::visit(
        [&event_types](const auto &evt) {
          using T = std::decay_t<decltype(evt)>;
          if constexpr (std::is_same_v<T, EntityDiedEvent>) {
            event_types.push_back("Died");
          } else if constexpr (std::is_same_v<T, EntityDamagedEvent>) {
            event_types.push_back("Damaged");
          } else if constexpr (std::is_same_v<T, EntityMovedEvent>) {
            event_types.push_back("Moved");
          }
        },
        e);
  });

  // Expected order: Immediate (Died), High (Damaged), Normal (Moved, Moved)
  EXPECT_EQ(event_types.size(), 4u);
  EXPECT_TRUE(event_types[0] == "Died");    // Immediate priority first
  EXPECT_TRUE(event_types[1] == "Damaged"); // High priority second
  EXPECT_TRUE(event_types[2] == "Moved");   // Normal priority third
  EXPECT_TRUE(event_types[3] == "Moved");   // Normal priority fourth (FIFO)

  std::cout << "  ✓ Events processed in priority order" << std::endl;
}

// Test FIFO within same priority
void testFIFOWithinPriority() {
  std::cout << "Testing FIFO within same priority..." << std::endl;

  EventQueue queue;

  // Push multiple events with same priority
  queue.push(TestEvent{1, "first"});
  queue.push(TestEvent{2, "second"});
  queue.push(TestEvent{3, "third"});
  queue.push(TestEvent{4, "fourth"});

  std::vector<int> values;

  queue.process([&values](const Event &e) {
    if (std::holds_alternative<TestEvent>(e)) {
      values.push_back(std::get<TestEvent>(e).value);
    }
  });

  // Should be processed in insertion order (FIFO)
  EXPECT_EQ(values.size(), 4u);
  EXPECT_EQ(values[0], 1);
  EXPECT_EQ(values[1], 2);
  EXPECT_EQ(values[2], 3);
  EXPECT_EQ(values[3], 4);

  std::cout << "  ✓ FIFO order preserved within same priority" << std::endl;
}

// Test empty queue processing
void testEmptyQueueProcess() {
  std::cout << "Testing empty queue processing..." << std::endl;

  EventQueue queue;

  bool callback_called = false;
  queue.process([&callback_called](const Event &) { callback_called = true; });

  EXPECT_FALSE(callback_called);
  EXPECT_TRUE(queue.empty());

  std::cout << "  ✓ Processing empty queue is safe" << std::endl;
}

// Test getPriority function
void testGetPriority() {
  std::cout << "Testing getPriority function..." << std::endl;

  Event died_event = EntityDiedEvent{1, "test", {0, 0}, -1, "fall"};
  Event damaged_event = EntityDamagedEvent{2, 1, 10, 90, "physical"};
  Event moved_event = EntityMovedEvent{3, {0, 0}, {1, 1}};
  Event transition_event =
      LevelTransitionEvent{LevelTransitionEvent::Down, 1, 2, {5, 5}};

  EXPECT_TRUE(getPriority(died_event) == EventPriority::Immediate);
  EXPECT_TRUE(getPriority(damaged_event) == EventPriority::High);
  EXPECT_TRUE(getPriority(moved_event) == EventPriority::Normal);
  EXPECT_TRUE(getPriority(transition_event) == EventPriority::Immediate);

  std::cout << "  ✓ getPriority returns correct priorities" << std::endl;
}

// Test event data integrity
void testEventDataIntegrity() {
  std::cout << "Testing event data integrity..." << std::endl;

  EventQueue queue;

  // Create complex event with all fields
  EntityDiedEvent died_evt{
      123,               // entity_id
      "Ancient Dragon",  // name
      Position{42, 17},  // position
      456,               // killer_id
      "heroic sacrifice" // cause_of_death
  };

  queue.push(died_evt);

  queue.process([](const Event &e) {
    EXPECT_TRUE(std::holds_alternative<EntityDiedEvent>(e));

    const auto &evt = std::get<EntityDiedEvent>(e);
    EXPECT_EQ(evt.entity_id, 123);
    EXPECT_TRUE(evt.name == "Ancient Dragon");
    EXPECT_EQ(evt.position.x, 42);
    EXPECT_EQ(evt.position.y, 17);
    EXPECT_EQ(evt.killer_id, 456);
    EXPECT_TRUE(evt.cause_of_death == "heroic sacrifice");
  });

  std::cout << "  ✓ Event data preserved through queue" << std::endl;
}

// Test LevelTransitionEvent directions
void testLevelTransitionDirections() {
  std::cout << "Testing LevelTransitionEvent directions..." << std::endl;

  EventQueue queue;

  queue.push(LevelTransitionEvent{LevelTransitionEvent::Down, 1, 2, {10, 10}});

  queue.push(LevelTransitionEvent{LevelTransitionEvent::Up, 5, 4, {15, 15}});

  int down_count = 0;
  int up_count = 0;

  queue.process([&](const Event &e) {
    if (std::holds_alternative<LevelTransitionEvent>(e)) {
      const auto &evt = std::get<LevelTransitionEvent>(e);
      if (evt.direction == LevelTransitionEvent::Down) {
        down_count++;
        EXPECT_EQ(evt.from_depth, 1);
        EXPECT_EQ(evt.to_depth, 2);
      } else {
        up_count++;
        EXPECT_EQ(evt.from_depth, 5);
        EXPECT_EQ(evt.to_depth, 4);
      }
    }
  });

  EXPECT_EQ(down_count, 1);
  EXPECT_EQ(up_count, 1);

  std::cout << "  ✓ LevelTransition directions work correctly" << std::endl;
}

int main() {
  std::cout << "\n=== Event System Tests ===" << std::endl;

  try {
    // Basic functionality
    testPushAndQuery();
    testClear();
    testProcess();

    // Priority and ordering
    testPriorityOrdering();
    testFIFOWithinPriority();
    testGetPriority();

    // Edge cases
    testEmptyQueueProcess();

    // Event data
    testEventDataIntegrity();
    testLevelTransitionDirections();

    std::cout << "\n✓ All tests passed!" << std::endl;
    return 0;

  } catch (const std::exception &e) {
    std::cerr << "\n✗ Test failed with exception: " << e.what() << std::endl;
    return 1;
  }
}
