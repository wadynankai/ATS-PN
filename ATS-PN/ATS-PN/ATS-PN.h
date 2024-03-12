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
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <chrono>
#include "atsplugin.h"
#include "..\..\common\LoadBveText.h"
#include "CATSPN.h"
#include "CDoorcontrol.h"
#include "CAutoAnnounce.h"
#include "CTraponBackGround.h"
#include "..\..\common\CAtsSound.h"
#include "..\..\common\CSourceVoice.h"

#ifdef EXCEPTION
#include <source_location>
#endif // EXCEPTION

using namespace std::literals::chrono_literals;


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
inline int g_svcBrake;//��p�ő�
inline int g_emgBrake;//���u���[�L
inline int g_Power;//�͍s�m�b�`
inline int g_Brake;//�u���[�L�m�b�`
inline int g_Reverser;//���o�[�T
inline float g_TrainSpeed;//���x
inline std::chrono::time_point<std::chrono::milliseconds> g_time;//����
inline std::chrono::milliseconds g_deltaT;//�O�̃t���[���Ƃ̎����̍�
inline double g_location;
inline double g_deltaL;//1�t���[���Ői�񂾋���
inline std::chrono::milliseconds g_tmr;//��ԉw���_�Ń^�C�}�[
inline int g_StopSta;//��ԉw��
inline std::chrono::milliseconds g_AstTimer;//�A�X�^���X�N�_�Ń^�C�}�[
inline bool g_Aster;//�A�X�^���X�N
inline int g_timetable = 0;//�����\
inline bool g_home_push = false;//Home��������Ă����True
inline bool g_insert_push = false;//Insert��������Ă����True
inline bool g_delete_push = false;//Delete��������Ă����True
inline bool g_PgUp_push = false;//Delete��������Ă����True
inline bool g_PgDn_push = false;//Delete��������Ă����True

inline winrt::com_ptr<IXAudio2> pXAudio2;
inline IXAudio2MasteringVoice* pMasteringVoice = nullptr;
inline std::filesystem::path g_module_dir;
inline constexpr winrt::Windows::Foundation::TimeSpan dingDuration = 42ms;
inline bool g_Space = false;

inline CSourceVoice g_Ding = nullptr, g_Ding1 = nullptr, g_Ding2 = nullptr;
//inline std::array<int, 2> g_sta_no = { 0,0 };
inline bool g_ShasyouBell = false;//�ԏ��x���@�\�L��
inline bool g_Bell1 = false;
inline bool g_Bell2 = false;
inline std::chrono::milliseconds g_belltimer = 0ms;

inline CAtsSound<26> g_trapon_push;
inline CAtsSound<27> g_trapon_release;
inline CAtsSound<28> g_trapon_on;
inline CAtsSound<29> g_trapon_off;


#endif // !_ATS_PN_INCLUDED_