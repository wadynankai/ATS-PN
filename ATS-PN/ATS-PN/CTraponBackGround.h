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
	std::vector<std::pair<size_t,size_t>> m_Train_no{};//�����\�ԍ��̃��X�g(first:panel�i�z��j�̃C���f�b�N�X�Csecond:�摜�̃C���f�b�N�X�j
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
		m_transitionTime = 500 / count;
		m_Train_no.clear();
		m_power = false;
	}
	void changeColor()noexcept { m_white_ref = !m_white_ref; }//�\���ؑփ{�^��
	bool powerButton()noexcept //�d���{�^��
	{
		m_white_ref = false;
		m_white = false;
		m_count = 1;
		m_index = 0;
		m_power = !m_power; 
		return m_power;
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
	void addTrainNo(size_t index,size_t dia) noexcept
	{ 
		std::pair<size_t, size_t> temp{ index,dia };
		if (std::none_of(m_Train_no.cbegin(), m_Train_no.cend(), [&](std::pair<size_t, size_t> x) { return x == temp; }))
		{
			m_Train_no.emplace_back(temp);
		}
	}
	_NODISCARD const std::pair<size_t,size_t> getTimeTable() const noexcept
	{
		if (!m_power)//�g���|���̓d�����؂�Ă���Ƃ�
		{
			return { 100,0 };//�����摜

		}
		else [[likely]]
		{
			if (!m_IcCard || m_Train_no.empty())//IC�J�[�h���������Ă��Ȃ��Ƃ�
			{
				if (!m_white)
				{
					return { 100,1 };
				}
				else
				{
					return { 100,2 };
				}
			}
			else [[likely]]//IC�J�[�h���������Ă���Ƃ�
			{

				switch (m_Train_no.at(m_index).first)
				{
				case 100:
					if (!m_white)
					{
						return { m_Train_no.at(m_index).first,2 * m_Train_no.at(m_index).second + 1 };
					}
					else
					{
						return { m_Train_no.at(m_index).first,2 * m_Train_no.at(m_index).second + 2 };
					}
					break;
				default:
					if (!m_white)
					{
						return { m_Train_no.at(m_index).first,2 * m_Train_no.at(m_index).second - 1 };
					}
					else
					{
						return { m_Train_no.at(m_index).first,2 * m_Train_no.at(m_index).second };
					}
					break;
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
	_NODISCARD const bool getColor()const noexcept { return m_white; }
	_NODISCARD const bool getPower()const noexcept { return m_power; }
	_NODISCARD const int getBackGround(const int deltaT)noexcept
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