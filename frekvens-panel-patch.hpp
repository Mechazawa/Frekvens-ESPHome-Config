#pragma once

bool Panel::get_pixel(int x, int y) {
  uint16_t pos = x + (y * this->get_width_internal());
  
  return this->buffer_[pos];
}
