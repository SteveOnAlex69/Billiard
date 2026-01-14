#ifndef MEDIAPLAYER_HPP
#define MEDIAPLAYER_HPP
#include <SFML/Audio.hpp>
#include <vector>
#include <filesystem>

enum SoundEffect {
	CLACK = 0,
	DING = 1
};

class MediaPlayer {
public:

	void init();
	void play_background_music();
	void play_audio(SoundEffect cur_song);

	void setMusicVolume(int volume);
	void setAudioVolume(int volume);

	bool setAudioType(int t);
private:
	sf::Music background_music;
	std::vector<sf::SoundBuffer> audio_buffer_list;
	std::vector<sf::Sound> audio_list;

	int current_type;
};

#endif