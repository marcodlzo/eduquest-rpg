CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
SRC = src/main.cpp src/Game.cpp src/Player.cpp src/Enemy.cpp src/BattleSystem.cpp src/QuestionManager.cpp src/UIManager.cpp src/GameManager.cpp src/SoundManager.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = eduquest

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LDFLAGS)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
