#include <GL/glut.h>
#include <GL/glu.h>
#include <string.h>
#include <stdio.h>

#include "GEVisual.h"

unit_rect_t unit_point_rect(unit_vector_t cell)
{
  unit_rect_t rect;
  rect.start = cell;
  rect.end = rect.start;
  return rect;
}

unit_rect_t unit_point_rect(int x, int y, int z)
{
  unit_rect_t rect;
  rect.start.x = x;
  rect.start.y = y;
  rect.start.z = z;
  rect.end = rect.start;
  return rect;
}

cell_rect_t cell_point_rect(cell_vector_t cell)
{
  cell_rect_t rect;
  rect.start = cell;
  rect.end = rect.start;
  return rect;
}

cell_rect_t cell_point_rect(int x, int y)
{
  cell_rect_t rect;
  rect.start.col = x;
  rect.start.row = y;
  rect.end = rect.start;
  return rect;
}

cell_vector_t empty_cell_vector = {-1, -1};
cell_rect_t empty_cell_rect = {-1, -1, -1, -1};

GEVisual::GEVisual()
{
  this->bg_color = COLOR_WHITE;
  this->onTouch = NULL;
  this->position.x = 0;
  this->position.y = 0;
  this->position.z = 0;
}

GEVisual::~GEVisual()
{
}

void GEVisual::draw(vector_t unit_size)
{
  glPushMatrix();
  if (key != 0) {
    glLoadName(key);
    //printf("Draw key %d\n", key);
  }
  glTranslatef(unit_size.x * position.x,
      unit_size.y * position.y,
      unit_size.z * position.z);
  //printf("%f %f %f\n", unit_size.x * position.x,
  //      unit_size.y * position.y,
  //    unit_size.z * position.z);
  /*glRotatef(rotation.x, 1, 0, 0);
  glRotatef(rotation.y, 0, 1, 0);
  glRotatef(rotation.z, 0, 0, 1);*/
  drawVisual(unit_size);
  glPopMatrix();
}

void GEVisual::touch(int key_event, int type)
{
  if (onTouch != NULL)
    onTouch(this, key_event, type, opaque);
}

GESphere::GESphere(int radius)
{
  this->radius = radius;
}

GESphere::~GESphere()
{
}

int GESphere::get_radius()
{
  return this->radius;
}

void GESphere::drawVisual(vector_t unit_size)
{
  glColor3ub(RED(bg_color), GREEN(bg_color), BLUE(bg_color));
  glutSolidSphere(radius * min_dimension(unit_size), 100, 100);
}

GEVisual *GESphere::clone()
{
  GESphere *ret;
  ret = new GESphere(this->radius);
  return ret;
}

GEQuad::GEQuad(unit_vector_t dimensions)
{
  this->dimensions = dimensions;
}

GEQuad::~GEQuad()
{
}

unit_vector_t GEQuad::get_dimensions()
{
  return this->dimensions;
}

void GEQuad::drawVisual(vector_t unit_size)
{
  float hx, hy, hz;
  hx = (this->dimensions.x * unit_size.x) / 2;
  hy = (this->dimensions.y * unit_size.y) / 2;
  hz = (this->dimensions.z * unit_size.z) / 2;
  glBegin(GL_QUADS);
  glColor3ub(RED(bg_color), GREEN(bg_color), BLUE(bg_color));
  glVertex3f( hx, hy,-hz);
  glVertex3f(-hx, hy,-hz);
  glVertex3f(-hx, hy, hz);
  glVertex3f( hx, hy, hz);
  glVertex3f( hx,-hy, hz);
  glVertex3f(-hx,-hy, hz);
  glVertex3f(-hx,-hy,-hz);
  glVertex3f( hx,-hy,-hz);
  glVertex3f( hx, hy, hz);
  glVertex3f(-hx, hy, hz);
  glVertex3f(-hx,-hy, hz);
  glVertex3f( hx,-hy, hz);
  glVertex3f( hx,-hy,-hz);
  glVertex3f(-hx,-hy,-hz);
  glVertex3f(-hx, hy,-hz);
  glVertex3f( hx, hy,-hz);
  glEnd();
}

GEVisual *GEQuad::clone()
{
  GEQuad *ret;
  ret = new GEQuad(this->dimensions);
  return ret;
}

GEComposite::GEComposite(int size)
{
  this->size = size;
  this->components = (GEVisualComponent **) malloc(sizeof(GEVisualComponent *) * size);
  this->cursor = 0;
  memset(this->components, this->size, 0);
}

GEComposite::~GEComposite()
{
  free(this->components);
}

void GEComposite::resize(int new_size)
{
  int  diff;
  if (cursor > new_size) {
    cursor = new_size;
    return;
  }

  diff = new_size - size;
  if (diff <= 0)
    return;

  if (this->size < 256)
    this->size *= 2;
  else
    this->size += 256;
  this->size += diff;

  this->components = (GEVisualComponent **)realloc(this->components, this->size);
}

void GEComposite::addComponent(GEVisualComponent *component)
{
  resize(this->cursor + 1);
  this->components[this->cursor] = component;
  this->cursor++;
}

GEVisualComponent *GEComposite::getComponent(int i)
{
  if (i < 0 || i >= this->cursor)
    return NULL;
  return this->components[i];
}

void GEComposite::drawVisual(vector_t unit_size)
{
  int i;
  for (i = 0; i < this->cursor; i++)
    this->components[i]->draw(unit_size);
}

GEVisual* GEComposite::clone()
{
  return NULL;
}

GEMapGrid::GEMapGrid(cell_vector_t dimensions, unit_vector_t cell_size)
{
  cell_vector_t pos;
  this->onOver = NULL;
  this->onSelect = NULL;
  this->onCellDraw = NULL;
  this->cursor = empty_cell_vector;
  this->selection= empty_cell_rect;
  this->dimensions = dimensions;
  this->cell_size = cell_size;
  this->cell_size.z = 0;
  this->cells = new GEMapCell[dimensions.col * dimensions.row];
  this->select_dimensions.col = dimensions.col / 10;
  this->select_dimensions.row = dimensions.row / 10;

  for (int i = 0; i < dimensions.col * dimensions.row; i++) {
    pos.row = i / dimensions.col;
    pos.col = i % dimensions.col;
    this->cells[i] = GEMapCell(pos, this);
    this->cells[i].registerVisual();
  }
}

GEMapGrid::~GEMapGrid()
{
}

void GEMapGrid::moveCursorTo(cell_vector_t cell)
{
  if (!equals_cell_vector(cell, cursor)) {
    cursor = cell;
    if (this->onOver)
      this->onOver(this, cursor, opaque);
  }
}

void GEMapGrid::moveCursorTo(GEMapCell *cell)
{
  moveCursorTo(cell->map_position);
}

cell_vector_t GEMapGrid::getCursor()
{
  return cursor;
}

GEMapCell *GEMapGrid::getCellUnderCursor()
{
  if (cursor.col == -1 || cursor.row == -1)
    return NULL;
  return &cells[cursor.row * dimensions.col  + cursor.col];
}

void GEMapGrid::select(cell_rect_t area)
{
  this->selection = area;
  if (this->onSelect)
    this->onSelect(this, opaque);
}

void GEMapGrid::selectUnderCursor()
{
  select(cell_point_rect(cursor));
}

void GEMapGrid::clearSelection()
{
  select(empty_cell_rect);
}

cell_rect_t GEMapGrid::getSelection()
{
  return selection;
}

int GEMapGrid::isSelected(cell_vector_t cell)
{
  int tmp;
  cell_vector_t start, end;
  start = selection.start;
  end = selection.end;
  if (start.col < 0 || start.row < 0)
    return 0;
  if (end.col < 0 || end.row < 0)
    end = cursor;
  if (start.col > end.col) {
    tmp = start.col;
    start.col = end.col;
    end.col = tmp;
  }
  if (start.row > end.row) {
    tmp = start.row;
    start.row = end.row;
    end.row = tmp;
  }
  //printf("selection %d %d -> %d %d: %d %d\n", start.col, start.row, end.col, end.row, cell.col, end.col);
  return start.col <= cell.col && cell.col <= end.col && start.row <= cell.row && cell.row <= end.row;
}
void GEMapGrid::drawVisual(vector_t unit_size)
{
  cell_vector_t area;
  area.col = area.row = 5;
  //drawVisual(unit_size, area);
  drawSelectionGrid(unit_size);
}

void GEMapGrid::drawSelectionGrid(vector_t unit_size)
{
  float hx, hy, hz;
  hx = (cell_size.x * select_dimensions.col * unit_size.x) / 2;
  hy = (cell_size.y * select_dimensions.col * unit_size.y) / 2;

  glBegin(GL_QUADS);
  glVertex3f( hx, hy, 0);
  glVertex3f(-hx, hy, 0);
  glVertex3f(-hx,-hy, 0);
  glVertex3f( hx,-hy, 0);
  glEnd();
}

void GEMapGrid::drawVisual(vector_t unit_size, cell_vector_t draw_area)
{
  cell_draw_data_t ev_data;
  int i, stride, width, height, end;
  GEMapCell *under_cursor;
  end = dimensions.col * dimensions.row;
  if (draw_area.col < 0 || draw_area.row < 0) {
    i = 0;
    stride = 0;
    height = dimensions.row;
    width = dimensions.col;
  } else {
    i = draw_area.row * select_dimensions.row * dimensions.col + draw_area.col * select_dimensions.col;
    height = select_dimensions.row;
    width = select_dimensions.col;
    stride = dimensions.col - width;
  }
  under_cursor = getCellUnderCursor();
  // printf("Drawing map... %p \n", under_cursor);
  for (int row = 0; row < height && i < end; row++, i += stride) {
    for (int col = 0; col < width && i < end; col++, i++) {
      //printf("Drawing cell %d...\n", i);
      ev_data.bg_color = COLOR_RED;
      ev_data.cell = cells[i].map_position;
      ev_data.cell_height = 0;
      ev_data.delete_visual = 0;
      ev_data.visual = NULL;
      if (isSelected(cells[i].map_position))
	ev_data.bg_color = COLOR_BLUE;
      if (&cells[i] == under_cursor)
	ev_data.bg_color = COLOR_GREEN;
      if (onCellDraw)
	onCellDraw(this, ev_data, opaque);
      cells[i].bg_color = ev_data.bg_color;
      cells[i].height = ev_data.cell_height;
      cells[i].visual = ev_data.visual;
      cells[i].draw(unit_size);
      if (ev_data.visual && ev_data.delete_visual)
	delete ev_data.visual;
    }
  }
}

void GEMapGrid::touch(int key_event, int type, GEMapCell *cell)
{
  printf("KEY_EVENT %d\n", key_event);
  if (key_event != KEV_MOUSE_RIGHT) {
    switch (type) {
    case KEV_MOUSE_DOWN:
      selection.start = cell->map_position;
      selection.end = empty_cell_vector;
      break;
    case KEV_MOUSE_UP:
      selection.end = cell->map_position;
      select(selection);
      break;
    case KEV_MOUSE_OVER:
      printf("Mouse over\n");
      moveCursorTo(cell);
      break;
    }
  }
}
GEVisual* GEMapGrid::clone()
{
  return NULL;
}

GEMapCell::GEMapCell(){}

GEMapCell::GEMapCell(cell_vector_t map_position, GEMapGrid *map)
{
  this->map_position = map_position;
  this->parent = map;
  this->height = 0;
  this->position.x =  - map->dimensions.col * map->cell_size.x / 2
                      + map_position.col * map->cell_size.x;
  this->position.y =  map->dimensions.row * map->cell_size.y / 2
                      - map_position.row * map->cell_size.y;
  this->position.z = 0;
}

GEMapCell::~GEMapCell()
{
}

void GEMapCell::drawVisual(vector_t unit_size)
{
  float hx, hy, hz;
  hx = (this->parent->cell_size.x * unit_size.x) / 2 - unit_size.x/20;
  hy = (this->parent->cell_size.y * unit_size.y) / 2 - unit_size.y/20;
  hz = (this->height * unit_size.z) + 0.01;
  if (visual)
    visual->draw(unit_size);
  glBegin(GL_QUADS);
  glColor3ub(RED(bg_color), GREEN(bg_color), BLUE(bg_color));
  glVertex3f( hx, hy, 0);
  glVertex3f(-hx, hy, 0);
  glVertex3f(-hx, hy, hz);
  glVertex3f( hx, hy, hz);
  glVertex3f( hx,-hy, hz);
  glVertex3f(-hx,-hy, hz);
  glVertex3f(-hx,-hy, 0);
  glVertex3f( hx,-hy, 0);
  glVertex3f( hx, hy, hz);
  glVertex3f(-hx, hy, hz);
  glVertex3f(-hx,-hy, hz);
  glVertex3f( hx,-hy, hz);
  glVertex3f( hx,-hy, 0);
  glVertex3f(-hx,-hy, 0);
  glVertex3f(-hx, hy, 0);
  glVertex3f( hx, hy, 0);
  glEnd();
}

void GEMapCell::touch(int key_event, int type)
{
 printf("touch %d %d\n", map_position.col, map_position.row);
  parent->touch(key_event, type, this);
}

GEVisual* GEMapCell::clone()
{
  return NULL;
}
