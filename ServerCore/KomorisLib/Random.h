#pragma once

class Random
{
public:
	Random(VOID);
	virtual ~Random(VOID);

private:
	UINT	mSeed;

public:
	BOOL	Init(UINT seed);

	INT		Rand(VOID);
	INT		SimpleRandom(VOID);
	INT		ComplexRandom(VOID);
};