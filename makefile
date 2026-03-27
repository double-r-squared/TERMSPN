CXX      = g++
CXXFLAGS = -std=c++17 -Wall -I.
LIBS     = -lcurl -lncurses

TARGET = espn

HEADERS = \
	types.h \
	http.h \
	api/urls.h \
	api/parse.h \
	utils/assetLoader.h \
	utils/text.h \
	ui/ui_core.h \
	ui/screen_league.h \
	ui/screen_standings.h \
	ui/screen_team.h \
	ui/screen_news_list.h \
	ui/screen_news_detail.h

$(TARGET): main.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) main.cpp $(LIBS) -o $(TARGET)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)