#include <MediaPlayer.hpp>
#include <iostream>
#include <vector>

void MediaPlayer::init() {
	setAudioType(1);
}

void MediaPlayer::play_background_music() {
	background_music.stop();
	background_music.setLooping(true);
	background_music.play();
}

void MediaPlayer::play_audio(SoundEffect current_sound) {
	audio_list[int(current_sound)].play();
}

void MediaPlayer::setMusicVolume(int vol) {
	background_music.setVolume(vol);
}

void MediaPlayer::setAudioVolume(int vol) {
	for (auto& i : audio_list)
		i.setVolume(vol);
}

std::string to_string(int x) {
	if (x == 0) return "0";
	std::string res;
	while (x > 0) {
		res.push_back('0' + x % 10);
		x /= 10;
	}
	reverse(res.begin(), res.end());
	return res;
}

bool MediaPlayer::setAudioType(int t) {
	if (current_type == t) return false;

	current_type = t;
	audio_buffer_list.clear();
	audio_list.clear();

	const std::string PREFIX = std::string(PROJECT_DIR) + "assets/Sound/Pack" + to_string(t) + "/";
	
	/*
	const std::filesystem::path BACKGROUND_SONG =
		std::filesystem::absolute(PREFIX + "GameOST.ogg");
	if (!background_music.openFromFile(BACKGROUND_SONG.c_str())) {
		exit(1);
	}*/

	
	const std::filesystem::path CLACK_SOUND =
		std::filesystem::absolute(PREFIX + "Clack.ogg");
	audio_buffer_list.emplace_back(sf::SoundBuffer(CLACK_SOUND.c_str()));
	const std::filesystem::path DING_SOUND =
		std::filesystem::absolute(PREFIX + "Ding.ogg");
	audio_buffer_list.emplace_back(sf::SoundBuffer(DING_SOUND.c_str()));
	for (auto& i : audio_buffer_list) {
		audio_list.emplace_back(sf::Sound(i));
	}

	return true;
}