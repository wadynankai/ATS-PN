class CTraponBackGround
{
private:
	int m_transitionCount = 1;//�ω��̃R�}��
	int m_transitionTime = 1;//1�R�}�ω�����
	int m_timer = 0;
	int m_count = 0;
	bool m_power = false;//�g���|���d��
	bool m_white_ref = false;//�J���[�w�ߒl
	bool m_white = false;//���݂̃J���[
public:
	CTraponBackGround() = default;
	CTraponBackGround(int count) :m_transitionCount{ count }, m_transitionTime{ 1000 / count }{}
	~CTraponBackGround() = default;
	void setTransitionCount(int count) { m_transitionCount = count; m_transitionTime = 1000 / count; }
	void changeColor() { m_white_ref = !m_white_ref; }
	void powerButton() { m_power = !m_power; }
	void wakeUp() { m_power = true; }
	_NODISCARD bool getColor() { return m_white; }
	_NODISCARD bool getPower() { return m_power; }
	_NODISCARD int operator()(int deltaT)
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