/*
 * grepshortcutkeydialog.h - a dialog which greps a KeySequence and
 * emits a signal with this KeySequence as Parameter
 * Copyright (C) 2006 Cestonaro Thilo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
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

#include <QDialog>
#include <QKeyEvent>
#include <QKeySequence>

#include "ui_grepshortcutkeydialog.h"

#ifndef GREPSHORTCUTKEYDIALOG_H
#define GREPSHORTCUTKEYDIALOG_H

class GrepShortcutKeyDialog : public QDialog
{
	Q_OBJECT
public:
	GrepShortcutKeyDialog();

	// reimplemented
	void show();

protected:
	// reimplemented
	void keyPressEvent(QKeyEvent* event);
	void keyReleaseEvent(QKeyEvent* event);
	void closeEvent(QCloseEvent *event);

signals:
	void newShortcutKey(const QKeySequence& key);

private:
	Ui::GrepShortcutKeyDialog ui_;
	bool gotKey;

	void displayPressedKeys(const QKeySequence& keys);
	QKeySequence getKeySequence(QKeyEvent* event) const;
	bool isValid(int key) const;
	bool isModifier(int key) const;
};

#endif
