/*
 * This file is part of the PulseView project.
 *
 * Copyright (C) 2012 Joel Holdsworth <joel@airwebreathe.org.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include "cursorheader.h"

#include "ruler.h"
#include "view.h"

#include <QApplication>
#include <QFontMetrics>
#include <QMouseEvent>

#include <pv/widgets/popup.h>

using std::shared_ptr;

namespace pv {
namespace view {

const int CursorHeader::Padding = 20;
const int CursorHeader::BaselineOffset = 5;

int CursorHeader::calculateTextHeight()
{
	QFontMetrics fm(font());
	return fm.boundingRect(0, 0, INT_MAX, INT_MAX,
		Qt::AlignLeft | Qt::AlignTop, "8").height();
}

CursorHeader::CursorHeader(View &parent) :
	MarginWidget(parent),
	_dragging(false),
	_textHeight(calculateTextHeight())
{
	setMouseTracking(true);
}

QSize CursorHeader::sizeHint() const
{
	return QSize(0, _textHeight + Padding + BaselineOffset);
}

void CursorHeader::clear_selection()
{
	CursorPair &cursors = _view.cursors();
	cursors.first()->select(false);
	cursors.second()->select(false);
	update();
}

void CursorHeader::paintEvent(QPaintEvent*)
{
	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing);

	unsigned int prefix = pv::view::Ruler::calculate_tick_spacing(
		p, _view.scale(), _view.offset()).second;

	// Draw the cursors
	if (_view.cursors_shown()) {
		// The cursor labels are not drawn with the arrows exactly on the
		// bottom line of the widget, because then the selection shadow
		// would be clipped away.
		const QRect r = rect().adjusted(0, 0, 0, -BaselineOffset);
		_view.cursors().draw_markers(p, r, prefix);
	}
}

void CursorHeader::mouseMoveEvent(QMouseEvent *e)
{
	if (!(e->buttons() & Qt::LeftButton))
		return;

	if ((e->pos() - _mouse_down_point).manhattanLength() <
		QApplication::startDragDistance())
		return;

	_dragging = true;

	if (shared_ptr<TimeMarker> m = _grabbed_marker.lock())
		m->set_time(_view.offset() +
			((double)e->x() + 0.5) * _view.scale());
}

void CursorHeader::mousePressEvent(QMouseEvent *e)
{
	if (e->buttons() & Qt::LeftButton) {
		_mouse_down_point = e->pos();

		_grabbed_marker.reset();

		clear_selection();

		if (_view.cursors_shown()) {
			CursorPair &cursors = _view.cursors();
			if (cursors.first()->get_label_rect(
				rect()).contains(e->pos()))
				_grabbed_marker = cursors.first();
			else if (cursors.second()->get_label_rect(
				rect()).contains(e->pos()))
				_grabbed_marker = cursors.second();
		}

		if (shared_ptr<TimeMarker> m = _grabbed_marker.lock())
			m->select();

		selection_changed();
	}
}

void CursorHeader::mouseReleaseEvent(QMouseEvent *)
{
	using pv::widgets::Popup;

	if (!_dragging)
		if (shared_ptr<TimeMarker> m = _grabbed_marker.lock()) {
			Popup *const p = m->create_popup(&_view);
			const QPoint arrpos(m->get_x(), height() - BaselineOffset);
			p->set_position(mapToGlobal(arrpos), Popup::Bottom);
			p->show();
		}

	_dragging = false;
	_grabbed_marker.reset();
}

} // namespace view
} // namespace pv
