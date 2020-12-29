#ifndef _ATS_PN_INCLUDED_
#define _ATS_PN_INCLUDED_


#ifndef _WIN32_WINNT
#include <winsdkver.h>
#define _WIN32_WINNT _WIN32_WINNT_WIN10
//#define _WIN32_WINNT _WIN32_WINNT_WINBLUE
//#define _WIN32_WINNT _WIN32_WINNT_WIN8
//#define _WIN32_WINNT _WIN32_WINNT_WIN7
#endif //_WIN32_WINNT
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Windows �w�b�_�[����قƂ�ǎg�p����Ă��Ȃ����������O����
#endif
#define NOMINMAX
#include <Windows.h>
#include <mfapi.h>
#include <fstream>
#include <filesystem>
#include <memory>
#include <array>
#include <vector>

#include "atsplugin.h"
#include "CATSPN.h"
#include "CDoorcontrol.h"
#include "CAutoAnnounce.h"

namespace PN_Beacon
{
	inline constexpr int SpdLim = 6;//PN���䑬�x����
	inline constexpr int StopPatern = 12;//PN����w�ʖh�~
	inline constexpr int Defeat = 16;//PN���䑬�x��������
	inline constexpr int Terminal = 91;//�I�[�h��
	inline constexpr int Timetable = 100;//�����\�̕ύX
	inline constexpr int Halt = 105;//�w�ʖh�~�`���C��
	inline constexpr int LineLim = 130;//����ō����x
};

inline HMODULE g_hModule;
inline int g_svcBrake;//��p�ő�
inline int g_emgBrake;//���u���[�L
inline int g_Power;//�͍s�m�b�`
inline int g_Brake;//�u���[�L�m�b�`
inline int g_Reverser;//���o�[�T
inline float g_TrainSpeed;//���x
inline int g_time;//����
inline int g_deltaT;//�O�̃t���[���Ƃ̎����̍�
inline double g_location;
inline double g_deltaL;//1�t���[���Ői�񂾋���
inline int g_tmr;//��ԉw���_�Ń^�C�}�[
inline int g_StopSta;//��ԉw��
inline int g_AstTimer;//�A�X�^���X�N�_�Ń^�C�}�[
inline bool g_Aster;//�A�X�^���X�N
inline int g_timetable = 0;//�����\

inline winrt::com_ptr<IXAudio2> pXAudio2;
inline IXAudio2MasteringVoice* pMasteringVoice = nullptr;
inline std::filesystem::path g_module_dir;
inline CATSPN g_pncontrol;
inline std::unique_ptr<CDoorcontrol> g_door;
inline std::unique_ptr<CAutoAnnounce> g_announce;

inline ATS_HANDLES g_output;//�o��

#endif // !_ATS_PN_INCLUDED_