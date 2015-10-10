#pragma once 

class SubSystem {
public:
	SubSystem(){}
	~SubSystem(){}

	virtual void Startup() = 0;
	virtual void Shutdown() = 0;
	virtual void Update(const float deltaTime) = 0;
};