#pragma once

#include <QPushButton>
#include <memory>
#include <functional>

class ObservedPushButton :
	public QPushButton
{
    Q_OBJECT

public:
	ObservedPushButton( QWidget* parent = 0 );
	virtual ~ObservedPushButton();

	void connectPressed( std::function<void( void )> func );

private slots:
    void on_pressed( void );

private:
	struct Impl;
	std::unique_ptr<Impl> mImpl;
};

