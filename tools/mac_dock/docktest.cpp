/*
 * docktest.cpp: A test program for the dock class
 * Copyright (C) 2005  Remko Troncon
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * You can also redistribute and/or modify this program under the
 * terms of the Psi License, specified in the accompanied COPYING
 * file, as published by the Psi Project; either dated January 1st,
 * 2005, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#include <QApplication>
#include <QMessageBox>
#include <QPushButton>
#include <QLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>

#include "mac_dock.h"

class DockTestWidget : public QWidget
{
	Q_OBJECT

public:
    DockTestWidget( QWidget *parent=0);

public slots:
	void do_overlay();

private:
	QLineEdit *text;
};


DockTestWidget::DockTestWidget( QWidget *parent ) : QWidget(parent)
{
	// Initialize widgets
	QGridLayout *layout = new QGridLayout(this);

	layout->addWidget(new QLabel("Text",this),0,0);
	text = new QLineEdit(this);
	text->setText("1");
	layout->addWidget(text,0,1);

    QPushButton *overlay = new QPushButton( "Overlay", this );
	connect(overlay, SIGNAL(clicked()), SLOT(do_overlay()));
	layout->addWidget(overlay,1,0);
}

int main( int argc, char **argv )
{
    QApplication a( argc, argv );
    DockTestWidget w;
    w.show();
    return a.exec();
}
	
void DockTestWidget::do_overlay()
{
	MacDock::overlay(text->text());
}

#include "docktest.moc"
