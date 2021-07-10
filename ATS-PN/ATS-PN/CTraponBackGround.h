#include <algorithm>
#include <vector>
class CTrapon
{
private:
	CTrapon()noexcept = default;
	CTrapon& operator=(CTrapon&) = delete;
	CTrapon& operator=(CTrapon&&) = delete;
	~CTrapon()noexcept = default;
	int m_transitionCount = 1;//�ω��̃R�}��
	int m_transitionTime = 1;//1�R�}�ω�����
	int m_timer = 0;
	int m_count = 1;
	bool m_power = false;//�g���|���d��
	bool m_IcCard = false;//IC�J�[�h���
	bool m_white_ref = false;//�J���[�w�ߒl
	bool m_white = false;//���݂̃J���[
	std::vector<int> m_Train_no{};//�����\�ԍ��̃��X�g
	size_t m_index = 0;//���݂̎����\�C���f�b�N�X
public:
	static CTrapon& GetInstance()noexcept
	{
		static CTrapon instance;
		return instance;
	}
	void setTransitionCount(int count)noexcept
	{
		m_transitionCount = count;
		m_transitionTime = 1000 / count;
		m_Train_no.clear();
		m_power = false;
	}
	void changeColor()noexcept { m_white_ref = !m_white_ref; }//�\���ؑփ{�^��
	void powerButton()noexcept //�d���{�^��
	{
		m_white_ref = false;
		m_white = false;
		m_count = 1;
		m_index = 0;
		m_power = !m_power; 
	}
	void wakeUp()noexcept //�N��
	{
		if (!m_power)
		{
			m_white_ref = false;
			m_white = false;
			m_IcCard = true;
			m_index = 0;
			m_count = 1;
			m_power = true;
		}
	}
	void IcCard()noexcept { m_IcCard = !m_IcCard; }
	void addTrainNo(int no) noexcept
	{ 
		if (std::none_of(m_Train_no.cbegin(), m_Train_no.cend(), [&](int x) { return x == no; }))
		{
			m_Train_no.emplace_back(no);
		}
	}
	int getTimeTable()noexcept
	{
		if (!m_power)//�g���|���̓d�����؂�Ă���Ƃ�
		{
			return 0;//�����摜

		}
		else [[likely]]
		{
			if (!m_IcCard || m_Train_no.empty())//IC�J�[�h���������Ă��Ȃ��Ƃ�
			{
				if (!m_white)
				{
					return 1;
				}
				else
				{
					return 2;
				}
			}
			else [[likely]]//IC�J�[�h���������Ă���Ƃ�
			{
				if (!m_white)
				{
					return 2 * m_Train_no.at(m_index) + 1;
				}
				else
				{
					return 2 * m_Train_no.at(m_index) + 2;
				}
			}
		}
	}

	void nextTimeTable()noexcept//�i�ރ{�^��
	{
		if (!m_Train_no.empty() && m_index + 1 < m_Train_no.size())
		{
			++m_index;
		}
	}
	void prevTimeTable()noexcept//�߂�{�^��
	{
		if (!m_Train_no.empty() && m_index > 0)
		{
			--m_index;
		}
	}
	_NODISCARD bool getColor()noexcept { return m_white; }
	_NODISCARD bool getPower()noexcept { return m_power; }
	_NODISCARD int getBackGround(int deltaT)noexcept
	{
		if (!m_power)//�d����
		{
			m_count = 0;
			m_white_ref = false;
			m_white = false;
		}
		else if (m_count == 0) m_count = 1;//�d�����������u��
		else [[likely]]//�d����
		{
			if (!m_white_ref && m_count != 1)//���֑J��
			{
				m_timer += deltaT;
				if (m_timer >= m_transitionTime)//�߂���ꍇ
				{
					int no = (m_timer / m_transitionTime);
					m_count += no;
					if (m_count > 2 * m_transitionCount)
					{
						m_count = 1;
						m_white = false;
					}
					m_timer -= no * m_transitionTime;
				}
			}
			else if (m_white_ref && m_count != m_transitionCount + 1)//���֑J��
			{
				m_timer += deltaT;
				if (m_timer >= m_transitionTime)//�߂���ꍇ
				{
					int no = (m_timer / m_transitionTime);
					m_count += no;
					if (m_count >= m_transitionCount + 1)
					{
						m_count = m_transitionCount + 1;
						m_white = true;
					}
					m_timer -= no * m_transitionTime;
				}
			}
			else [[likely]] m_timer = 0;//�ω����Ă��Ȃ��Ƃ�
		}
		return m_count;
	}
};