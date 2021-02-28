class CTraponBackGround
{
private:
	int m_transitionCount = 1;//変化のコマ数
	int m_transitionTime = 1;//1コマ変化時間
	int m_timer = 0;
	int m_count = 0;
	bool m_power = false;//トラポン電源
	bool m_white_ref = false;//カラー指令値
	bool m_white = false;//現在のカラー
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
		if (!m_power)//電源切
		{
			m_count = 0;
			m_white_ref = false;
			m_white = false;
		}
		else if (m_count == 0) m_count = 1;//電源が入った瞬間
		else [[likely]]//電源入
		{
			if (!m_white_ref && m_count != 1)//黒へ遷移
			{
				m_timer += deltaT;
				if (m_timer >= m_transitionTime)//めくる場合
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
			else if (m_white_ref && m_count != m_transitionCount + 1)//白へ遷移
			{
				m_timer += deltaT;
				if (m_timer >= m_transitionTime)//めくる場合
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
			else [[likely]] m_timer = 0;//変化していないとき
		}
		return m_count;
	}
};