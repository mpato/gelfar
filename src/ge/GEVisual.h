#ifndef GEVISUAL_H
#define GEVISUAL_H
#include <math.h>
#include "../core/corevisual.h"
#include "../core/utils/vector.h"

extern cell_vector_t empty_cell_vector;
extern cell_rect_t empty_cell_rect;

#define equals_cell_vector(A,B) (A.col == B.col && A.row == B.row)
#define equals_cell_rect(A,B) (equals_cell_vector(A.start, B.start) && equals_cell_vector(A.end, B.end))

#define min_dimension(point) (fmin(fmin(point.x, point.y), point.z))

//Functions to convert a point (a vector) to a rectangle
unit_rect_t unit_point_rect(unit_vector_t c);
unit_rect_t unit_point_rect(int x, int y, int z);
cell_rect_t cell_point_rect(cell_vector_t c);
cell_rect_t cell_point_rect(int x, int y);

//Colors
typedef int GEColor;
#define RGB(r, g, b) ((r << 16) | (g << 8) | b)
#define RED(c) ((c >> 16) & 0xFF)
#define GREEN(c) ((c >> 8) & 0xFF)
#define BLUE(c) (c & 0xFF)
#define COLOR_BLACK (RGB(0x00, 0x00, 0x00))
#define COLOR_WHITE (RGB(0xFF, 0xFF, 0xFF))
#define COLOR_RED   (RGB(0xFF, 0x00, 0x00))
#define COLOR_GREEN (RGB(0x00, 0xFF, 0x00))
#define COLOR_BLUE  (RGB(0x00, 0x00, 0xFF))

class GEVisual;
typedef void (*ge_on_touch_f)(GEVisual *sender, int key_event, int type, void *opaque);

class GEVisual : public CoreVisual{
  protected:
    GEColor bg_color;
    void *opaque;
    virtual void drawVisual(vector_t unit_size) = 0;
  public:
    unit_vector_t position;
    vector_t rotation;
    ge_on_touch_f onTouch;
    GEVisual();
    virtual ~GEVisual();
    virtual void touch(int key_event, int type);
    virtual void draw(vector_t unit_size);
    virtual GEVisual *clone()=0;
};

class GEVisualComponent : public GEVisual{
  public:
    virtual GEVisual *clone()=0;
};

class GESphere : public GEVisualComponent {
  private:
    nunits_t radius;
  protected:
    virtual void drawVisual(vector_t unit_size);
  public:
    GESphere(int radius);
    virtual ~GESphere();
    int get_radius();
    virtual GEVisual *clone();
};

class GEQuad : public GEVisualComponent {
  private:
    unit_vector_t dimensions;
  protected:
    virtual void drawVisual(vector_t unit_size);
  public:
    GEQuad(unit_vector_t dimensions);
    virtual ~GEQuad();
    unit_vector_t get_dimensions();
    virtual GEVisual *clone();
};

class GEComposite : public GEVisualComponent{
  private:
    GEVisualComponent **components;
    int size;
    int cursor;
  protected:
    virtual void drawVisual(vector_t unit_size);
  public:
    GEComposite(int size);
    virtual ~GEComposite();
    void resize(int new_siznunits_te);
    void addComponent(GEVisualComponent *component);
    GEVisualComponent *getComponent(int i);
    virtual GEVisual *clone();
};

class GEMapCell;
class GEMapGrid;

struct cell_draw_data_t {
    GEColor bg_color;
    nunits_t cell_height;
    GEVisualComponent *visual;
    cell_vector_t cell;
    int delete_visual;
};

class GEMapGrid;
typedef void (*ge_on_celldraw_f) (GEMapGrid *map, cell_draw_data_t &event_data, void *opaque);
typedef void (*ge_on_over_f) (GEMapGrid *map, cell_vector_t cell, void *opaque);
typedef void (*ge_on_select_f) (GEMapGrid *map, void *opaque);

class GEMapGrid : public GEVisual {
  private:
    cell_rect_t selection;
    cell_vector_t cursor;
    unit_vector_t cell_size;
    cell_vector_t dimensions;
    cell_vector_t select_dimensions;
    GEMapCell *cells;
  protected:
    virtual void drawVisual(vector_t unit_size);
    void drawVisual(vector_t unit_size, cell_vector_t draw_area);
    void drawSelectionGrid(vector_t unit_size);
  public:
    ge_on_over_f onOver;
    ge_on_select_f onSelect;
    ge_on_celldraw_f onCellDraw;
    GEMapGrid(cell_vector_t dimensions, unit_vector_t cell_size);
    virtual ~GEMapGrid();
    void moveCursorTo(cell_vector_t cell);
    void moveCursorTo(GEMapCell *cell);
    GEMapCell *getCellUnderCursor();
    cell_vector_t getCursor();
    void select(cell_rect_t area);
    void selectUnderCursor();
    void clearSelection();
    cell_rect_t getSelection();
    int isSelected(cell_vector_t cell);
    virtual GEVisual *clone();
    void touch(int key_event, int type, GEMapCell *cell);
    friend class GEMapCell;
};

class GEMapCell : public GEVisual {
  protected:
    virtual void drawVisual(vector_t unit_size);
  public:
    GEMapGrid *parent;
  private :
    nunits_t height;
    cell_vector_t map_position;
    GEVisual *visual;
    GEMapCell();
    GEMapCell(cell_vector_t map_position, GEMapGrid *map);
    virtual ~GEMapCell();
    virtual GEVisual *clone();
    virtual void touch(int key_event, int type);
    friend class GEMapGrid;
};
#endif
