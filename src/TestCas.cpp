#include "gbos1936/Gbos1936.h"
#include <iostream>
#include <math.h>

HelmertConverter gConverter;

int main()
{


  std::cout << "Cassini" << std::endl;
  double e = 0.0, n = 0.0, h = 0.0;
  gConverter.ConvertWgs84ToCas(53.22175203,-2.68569717,0.0,e,n);
  std::cout << ( e * 5280 ) << "," << ( n * 5280 ) << std::endl;
  gConverter.ConvertWgs84ToCas(53.04493168,-2.74724954,0.0,e,n);
  std::cout << ( e * 5280 ) << "," << ( n * 5280 ) << std::endl;

  double lat = 0.0, lon = 0.0, alt = 0.0;
  gConverter.ConvertCasToWgs84(0.0,0.0,0.0,lat, lon, alt );
  std::cout << lat << "," << lon << std::endl;
  gConverter.ConvertCasToWgs84(100.0,100.0,0.0,lat, lon, alt );
  std::cout << lat << "," << lon << std::endl;

  std::cout << "Bonne" << std::endl;
  gConverter.ConvertWgs84ToBnS(57.49973044,-4.00138586,0.0,e,n);
  std::cout << ( e * 5280 ) << "," << ( n * 5280 ) << std::endl;

  gConverter.ConvertBnSToWgs84(0.0,0.0,0.0, lat, lon, alt );
  std::cout << lat << "," << lon << std::endl;
  gConverter.ConvertBnSToWgs84(100.0,100.0,0.0, lat, lon, alt );
  std::cout << lat << "," << lon << std::endl;

  std::cout << "OSI" << std::endl;
  gConverter.ConvertOsiToWgs84(200000,250000,0.0, lat, lon, alt );
  std::cout << lat << "," << lon << std::endl;
  gConverter.ConvertWgs84ToOsi(53.5,-8,0.0, e, n, h);
  std::cout << e << "," << n << std::endl;

  std::cout << "French Bonne" << std::endl;
  //gConverter.ConvertWgs84ToBnF(48.86, 2.33722222, 0.0, e, n );
  gConverter.ConvertWgs84ToBnF(48.836439, 2.336506, 0.0, e, n );
  std::cout << "Point zero " << e << "," << n << std::endl;

  gConverter.ConvertWgs84ToParis(48.836439, 2.336506, lat, lon);
  std::cout << lat << "," << lon << std::endl;

  gConverter.ConvertWgs84ToBnF(47.957873, -0.543229, 0.0, e, n );
  std::cout << "47.957873, -0.543229 " << e << "," << n << std::endl;
  gConverter.ConvertBnFToWgs84( -215, -93.5, 0.0, lat, lon, alt );
  std::cout << "-215, -93.5 " << lat << "," << lon << std::endl;

  gConverter.ConvertWgs84ToBnF( 47.95875392, -0.31017671, 0.0, e, n );
  std::cout << "47.95875392, -0.31017671 " << e << "," << n << std::endl;

  gConverter.ConvertWgs84ToBnF(47.955808, 5.214928, 0.0, e, n );
  std::cout << "47.955808, 5.214928 " << e << "," << n << std::endl;

  gConverter.ConvertBnFToWgs84( 215, -93.5, 0.0, lat, lon, alt );
  std::cout << "215, -93.5 " << lat << "," << lon << std::endl;
  
  gConverter.ConvertWgs84ToBnF( 43.205239, 0.559556, 0.0, e, n );
  std::cout << "43.205239, 0.559556 " << e << "," << n << std::endl;


  return 0;
}
