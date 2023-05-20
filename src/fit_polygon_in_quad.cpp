#include "kiwi/kiwi.h"
#include "croppy_ffi.h"
#include "fit_polygon_in_quad.h"
#include <vector>
#include <algorithm>
#include <chrono>

using namespace kiwi;

Solver solver;

void report_time(std::string label, std::chrono::steady_clock::time_point start)
{
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  std::cout << label << ": " << duration.count() << " microseconds" << std::endl;
}

Aabb2 fit_polygon_in_quad_impl(double *points, int length)
{
  // auto start = std::chrono::high_resolution_clock::now();

  solver.reset();
  std::vector<Vector2> quad_points_vec;

  for (int i = 0; i < 4; i++)
  {
    quad_points_vec.push_back(Vector2{points[2 * i], points[2 * i + 1]});
  }

  std::vector<Vector2> polygon_points_vec;
  for (int i = 0; i < (length - 8) / 2; i++)
  {
    polygon_points_vec.push_back(Vector2{points[8 + i * 2], points[9 + i * 2]});
  }

  Vector2 quad_min;
  Vector2 quad_max;

  for (int i = 0; i < 4; i++)
  {
    if (i == 0)
    {
      quad_min = quad_points_vec[i];
      quad_max = quad_points_vec[i];
    }
    else
    {
      quad_min = Vector2{min(quad_min.x, quad_points_vec[i].x), min(quad_min.y, quad_points_vec[i].y)};
      quad_max = Vector2{max(quad_max.x, quad_points_vec[i].x), max(quad_max.y, quad_points_vec[i].y)};
    }
  }

  Vector2 polygon_min;
  Vector2 polygon_max;

  for (int i = 0; i < polygon_points_vec.size(); i++)
  {
    if (i == 0)
    {
      polygon_min = polygon_points_vec[i];
      polygon_max = polygon_points_vec[i];
    }
    else
    {
      polygon_min = Vector2{min(polygon_min.x, polygon_points_vec[i].x), min(polygon_min.y, polygon_points_vec[i].y)};
      polygon_max = Vector2{max(polygon_max.x, polygon_points_vec[i].x), max(polygon_max.y, polygon_points_vec[i].y)};
    }
  }

  Vector2 polygon_size = Vector2{polygon_max.x - polygon_min.x, polygon_max.y - polygon_min.y};

  // report_time("setup", start);

  Variable out_x("out_x");
  Variable out_y("out_y");
  Variable out_a("out_a");

  solver.addConstraint(Constraint{out_x >= quad_min.x});
  solver.addConstraint(Constraint{out_x <= quad_max.x});
  solver.addConstraint(Constraint{out_y >= quad_min.y});
  solver.addConstraint(Constraint{out_y <= quad_max.y});

  solver.addConstraint(Constraint{out_a >= 0});
  solver.addConstraint(Constraint{out_a <= 1});

  // report_time("basic_constraints", start);

  for (int poly_i = 0; poly_i < polygon_points_vec.size(); poly_i++)
  {
    double dx = polygon_points_vec[poly_i].x - polygon_min.x;
    double dy = polygon_points_vec[poly_i].y - polygon_min.y;

    for (int i = 0; i < 4; i++)
    {
      int j = (i + 1) % 4;

      Vector2 quad_i = quad_points_vec[i];
      Vector2 quad_j = quad_points_vec[j];

      double quad_dx = quad_j.x - quad_i.x;
      double quad_dy = quad_j.y - quad_i.y;
      
      solver.addConstraint(Constraint{
        (quad_dx) * (out_y + out_a * dy - quad_i.y) -
        (quad_dy) * (out_x + out_a * dx - quad_i.x) <= 0
      });
    }
  }

  // report_time("fit_constraints", start);

  solver.addEditVariable(out_x, strength::weak);
  solver.addEditVariable(out_y, strength::weak);
  solver.addEditVariable(out_a, strength::strong);

  solver.suggestValue(out_x, polygon_min.x);
  solver.suggestValue(out_y, polygon_min.y);
  solver.suggestValue(out_a, 1);

  // report_time("objective1_start", start);

  solver.updateVariables();

  double result_a = out_a.value();

  // report_time("objective1", start);


  double result_x = out_x.value();
  double result_y = out_y.value();

  // report_time("total", start);

  return Aabb2{
      Vector2{result_x, result_y},
      Vector2{result_x + polygon_size.x * result_a, result_y + polygon_size.y * result_a},
  };
}
