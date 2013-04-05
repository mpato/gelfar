#include <GL/glut.h>
#include <GL/glu.h>
#include <string.h>

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
}

GEVisual::~GEVisual()
{
}

void GEVisual::draw(vector_t unit_size)
{
  glPushMatrix();
  glTranslatef(unit_size.x * position.x,
      unit_size.y * position.y,
      unit_size.z * position.z);
  glRotatef(rotation.x, 1, 0, 0);
  glRotatef(rotation.y, 0, 1, 0);
  glRotatef(rotation.z, 0, 0, 1);
  drawVisual(unit_size);
  glPopMatrix();
}

void GEVisual::touch(int key_event, int down)
{
  if (onTouch != NULL)
    onTouch(key_event, down, opaque);
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

  for (int i = 0; i < dimensions.col * dimensions.row; i++) {
    pos.row = i / dimensions.col;
    pos.col = i % dimensions.col;
    this->cells[i] = GEMapCell(pos, this);
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
      this->onOver(cursor, opaque);
  }
}

cell_vector_t GEMapGrid::getCellUnderCursor()
{
  return cursor;
}

void GEMapGrid::select(cell_rect_t area)
{
  this->selection = area;
  if (this->onSelect)
    this->onSelect(area, opaque);
}

void GEMapGrid::selectUnderCursor()
{
  select(cell_point_rect(cursor));
}

cell_rect_t GEMapGrid::getSelection()
{
  return selection;
}

void GEMapGrid::drawVisual(vector_t unit_size)
{
  cell_draw_event_data_t ev_data;
  for (int i = 0; i < dimensions.col * dimensions.row; i++) {
    ev_data.bg_color = COLOR_WHITE;
    ev_data.cell = cells[i].map_position;
    ev_data.cell_height = 0;
    ev_data.delete_visual = 0;
    ev_data.map = this;
    ev_data.visual = NULL;
    if (onCellDraw)
      onCellDraw(ev_data, opaque);
    cells[i].bg_color = ev_data.bg_color;
    cells[i].height = ev_data.cell_height;
    cells[i].visual = ev_data.visual;
    cells[i].drawVisual(unit_size);
    if (ev_data.visual && ev_data.delete_visual)
      delete ev_data.visual;
  }
}

void GEMapGrid::touch(int key_event, int down, GEMapCell *cell)
{
  if (key_event == KEV_MOUSE_LEFT) {
   if (down) {
     selection.start = cell->map_position;
     selection.end = empty_cell_vector;
   } else {
     selection.end = cell->map_position;
     select(selection);
   }
  }
}

void cell_touch(int key_event, int down, void *opaque)
{
  GEMapCell *cell;
  cell = (GEMapCell *) opaque;
  cell->parent->touch(key_event, down, cell);
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
  this->onTouch = cell_touch;
}

GEMapCell::~GEMapCell()
{
}

void GEMapCell::drawVisual(vector_t unit_size)
{
  float hx, hy, hz;
   hx = (this->parent->cell_size.x * unit_size.x) / 2;
   hy = (this->parent->cell_size.y * unit_size.y) / 2;
   hz = (this->height * unit_size.z) / 2;
   if (visual)
     visual->draw(unit_size);
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
