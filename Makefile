# Compiler
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic

# Targets
all: test_robot Arena libRobot_Speedy.so libRatboy.so libFlame_e_o.so

Arena: Arena.cpp RobotBase.o
	$(CXX) $(CXXFLAGS) Arena.cpp RobotBase.o -ldl -o Arena

libRobot_Speedy.so: Robot_Speedy.cpp RobotBase.o
	$(CXX) $(CXXFLAGS) -fPIC -shared Robot_Speedy.cpp RobotBase.o -o libRobot_Speedy.so

libRatboy.so: Robot_Ratboy.cpp RobotBase.o
	$(CXX) $(CXXFLAGS) -fPIC -shared Robot_Ratboy.cpp RobotBase.o -o libRatboy.so

libFlame_e_o.so: Robot_Flame_e_o.cpp RobotBase.o
	$(CXX) $(CXXFLAGS) -fPIC -shared Robot_Flame_e_o.cpp RobotBase.o -o libFlame_e_o.so

RobotBase.o: RobotBase.cpp RobotBase.h
	$(CXX) $(CXXFLAGS) -c RobotBase.cpp

test_robot: test_robot.cpp RobotBase.o
	$(CXX) $(CXXFLAGS) test_robot.cpp RobotBase.o -ldl -o test_robot

clean:
	rm -f *.o test_robot *.so
