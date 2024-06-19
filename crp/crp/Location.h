#pragma once
class Location
{
private:
	int bay;
	int stack;
public:
	Location(int bay, int stack);
	int GetBay() const;
	int GetStack() const;
	void SetBay(int bay);
	void SetStack(int stack);
	void SetLocation(int bay, int stack);
	bool operator==(const Location& location) const;
	bool operator!=(const Location& location) const;
	bool operator<(const Location& location) const;
};

