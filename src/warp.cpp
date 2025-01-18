// CGAL 相关
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Point_3.h>
#include <CGAL/Point_2.h>
#include <CGAL/Vector_3.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/boost/graph/graph_traits_Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/compute_normal.h>
#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_traits.h>
#include <CGAL/AABB_face_graph_triangle_primitive.h>
#include <CGAL/Surface_mesh_shortest_path.h>
#include <CGAL/boost/graph/graph_traits_Polyhedron_3.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Polyhedron_items_with_id_3.h>
#include <CGAL/Surface_mesh_shortest_path.h>

// OpenCASCADE 相关
#include <opencascade/gp_Pnt.hxx>
#include <opencascade/gp_Vec.hxx>
#include <opencascade/gp_Dir.hxx>
#include <opencascade/TopoDS_Wire.hxx>
#include <opencascade/gp_Ax1.hxx>
#include <opencascade/Precision.hxx>

// Boost 相关
#include <boost/graph/graph_traits.hpp>
#include <boost/optional/optional.hpp>
#include <boost/variant.hpp>

// C++ 标准库
#include <vector>
#include <list>
#include <cmath>
#include <cassert>

typedef CGAL::Simple_cartesian<double> clKernel;      
typedef CGAL::Point_3<clKernel>        clPoint;       //点
typedef CGAL::Point_2<clKernel>        clPoint2d;     //2d点
typedef CGAL::Surface_mesh<clPoint>    clMesh;        //面网格
typedef CGAL::Polyhedron_3<clKernel>   clPolyhedron3; //
typedef CGAL::Vector_3<clKernel>       cgalVector;     //向量
typedef boost::graph_traits<clPolyhedron3_withId>    cgalPGraph_traits;
typedef cgalPGraph_traits::vertex_iterator           P_vertex_iterator;
typedef cgalPGraph_traits::edge_iterator         P_halfedge_iterator;
typedef boost::graph_traits<Polyhedron>::face_iterator P_face_iterator;
typedef CGAL::Exact_predicates_inexact_constructions_kernel     clKernel_e;
typedef CGAL::Polyhedron_3<clKernel_e,CGAL::Polyhedron_items_with_id_3> clPolyhedron3_withId;
//AABB树
typedef CGAL::AABB_face_graph_triangle_primitive<clMesh, CGAL::Default, CGAL::Boolean_tag<true>, CGAL::Boolean_tag<false>>         clMeshPrimitive;
typedef CGAL::AABB_traits<clKernel, clMeshPrimitive, CGAL::Default>           clAABB_Mesh_traits;
typedef CGAL::AABB_tree<clAABB_Mesh_traits>                    clMeshTree;

gp_Pnt toOccPoint(const clPoint& pnt)
{
    return gp_Pnt(pnt.x(), pnt.y(), pnt.z());
}
 
clPoint toCgalPoint(const gp_Pnt& pnt)
{
    return clPoint(pnt.X(), pnt.Y(), pnt.Z());
}

gp_Vec toOccVector(const cgalVector& vec)
{
    return gp_Vec(vec.x(), vec.y(), vec.z());
}

cgalVector toCgalVector(const gp_Vec& vec)
{
    return cgalVector(vec.X(), vec.Y(), vec.Z());
}

 
 //获取三角面法向
clPoint getNormal(const clPolyhedron3_withId& polyhedron, const P_face_iterator& face_it)
{
    //法向
    Traits::Vector_3 norV = CGAL::Polygon_mesh_processing::compute_face_normal(*face_it, polyhedron);
    return clPoint(norV.x(), norV.y(), norV.z());
}

//获取网格中距离给定点最近的点、点法线、所在三角面
bool getNeareastPoint1(const clPolyhedron3_withId& polyhedron, const Surface_mesh_shortest_path& shortPath, const clPoint& pnt, clPoint &relPnt, clPoint &relNormalDir, P_face_iterator& face_it)
{

      Traits::Point_3 sp(pnt.x(), pnt.y(), pnt.z());
      Surface_mesh_shortest_path::Face_location sfl = shortPath.locate<cgalAABB_Polyhedron3_traits>(sp);
      face_it = sfl.first;
      if (face_it == faces(polyhedron).end())
        return false;
	
      //triangleIndex = (*face_it)->id();
      //最近点
      sp = shortPath.point(sfl.first, sfl.second);
      relPnt = clPoint(sp.x(), sp.y(), sp.z());
      //法向
      relNormalDir = getNormal(polyhedron, face_it);
      return true;
  }

  void wrapWireOnMesh(
    const std::vector<TopoDS_Wire>& WS,
    const clPolyhedron3_withId& polyhedron,
    const gp_Dir& projectDir,
    std::vector< std::vector<gp_Ax1>>& wirePnts,
    double divideTol,
    bool isIntersectWithBorderEdges)
{
    
//第一步：获取包覆起点
//初始化索引（如果使用suface mesh,不用此句，而是创建shortpath时创建id，参考第二个示例，但官方建议使用当前方法）
// initialize indices of vertices, halfedges and facets
CGAL::set_halfedgeds_items_id(polyhedron);
//构建测地线计算
Surface_mesh_shortest_path shortest_paths(polyhedron);
//遍历每个wire，依次包覆
      int wsCount = (int)WS.size();
      std::vector<gp_Ax1> wrapPnts;
      std::vector<P_face_iterator> wrapPntOnFaces;
      std::vector < std::vector<MyModel::DiscretePoint>> allCurvePnts;
      allCurvePnts.resize(wsCount);
      //所有曲线只找一个起点就可以了，可以解决选择多个wire，但有的wire找不到起点（因为投影不上去）
      double minDis = Precision::Infinite();
      gp_Pnt startPntOnWs, startPntOnProjWs;
      std::vector<gp_Pnt> intersectPnts;
      std::vector<double> distances;
      cgalVector rayDir = MMesh_MeshFuns::toCgalVector(gp_Vec(projectDir));
      clMeshTree cgalMeshTree(faces(sm).first, faces(sm).second, sm);
      for (int i = 0; i < wsCount; i++)
      {
        const TopoDS_Wire& W = WS[i];
        //离散曲线(参考文章离散边为点列)
        DiscreteWireToPnts(W, allCurvePnts[i], divideTol, true, false, true, true,false);
        int count = (int)allCurvePnts[i].size();
        //1.找起点
        intersectPnts.resize(count);
        distances.resize(count, -1.0);
        for (int pi = 0; pi < count; pi++)
        {
          const gp_Pnt& checkPnt = allCurvePnts[i][pi].point;
          //射线求交
          cgalRay ray(toCgalPoint(checkPnt), rayDir);
          Ray_MeshIntersection intersection = cgalMeshTree.first_intersection(ray);
          if (intersection) {
            if (boost::get<clPoint>(&(intersection->first))) 
			{
              const clPoint* p = boost::get<clPoint>(&(intersection->first));
              gp_Pnt cp = toOccPoint(*p);
              intersectPnts[pi] = cp;
              distances[pi] = cp.Distance(checkPnt);
            }
          }
        }
		
		//获取距离最小的点
        int minIndex = -1;
        for (int pi = 0; pi < count; pi++)
        {
          const double& dis = distances[pi];
          if (dis >= 0 && dis < minDis)
          {
            minDis = dis;
            minIndex = pi;
          }
        }
		
        if (minIndex < 0)
          continue;
        startPntOnWs = allCurvePnts[i][minIndex].point;
        startPntOnProjWs = intersectPnts[minIndex];
	  }//第一步结束

     //第二部：依次包覆每条曲线
     /***********************************************************************/
     for (int i = 0; i < wsCount; i++)
     {
        const std::vector<MyModel::DiscretePoint>& curvePnts = allCurvePnts[i];
        wrapPnts.clear();
        wrapPntOnFaces.clear();
        int count = (int)curvePnts.size();
		//获取第一个相交面，并将第一个面铺到平面上
        P_face_iterator face_it_start;
        clPoint triNearPnt, triNormalDir;
        if (!getNeareastPoint1(polyhedron, shortest_paths, toCgalPoint(startPntOnProjWs), triNearPnt, triNormalDir, face_it_start))
          continue;
        if (toOccPoint(triNormalDir).XYZ().Modulus() < Precision::Confusion())
          continue;
        gp_Pnt pntOnTri_start = toOccPoint(triNearPnt);
        gp_Dir dirOnTri_start(toOccPoint(triNormalDir).XYZ());
		wrapPnts.resize(count);
		wrapPntOnFaces.resize(count);//为null则为该点包覆失败，可能已到边界，包覆曲线要分段
		for (int ci = 0; ci < count; ci++)
        {
          const gp_Pnt& curPnt = curvePnts[ci].point;
          P_face_iterator face_it = face_it_start;
          if (startPntOnWs.IsEqual(curPnt, Precision::Confusion()))
          {
            wrapPnts[ci] = gp_Ax1(pntOnTri_start, dirOnTri_start);
            wrapPntOnFaces[ci] = face_it;
            continue;
          }
          gp_Pnt pntOnTri = pntOnTri_start;
          scalePoint(pntOnTri, scale);
          gp_Dir dirOnTri = dirOnTri_start;
          //第一次将线段贴到三角面上的变换坐标系
          gp_Dir curXDir = gp_Dir(gp_Vec(startPntOnWs, curPnt));
          if (projectDir.IsParallel(curXDir, Precision::Angular()))
            continue;
          gp_Dir curYDir = projectDir.Crossed(curXDir);

          Standard_Real length = startPntOnWs.Distance(curPnt);
		  gp_Dir plnNormalDir = curYDir;//求交平面法向
          gp_Dir interLineDir = curXDir;//平面和法向的交线向量
          //不断循环将线段贴到网格上
          bool canContinue = true;
          while (length > 0 && canContinue)
          {
            canContinue = false;
            //平铺的变换怎么计算
            //1.求当前三角面的一个向量，该向量通过投影曲线当前点和切向量及投影方向创建的平面与三角面的交线
            //然后和法线创建一个坐标系，将该坐标系变换到投影点的坐标系
            P_halfedge_iterator hd1 = halfedge(*face_it, polyhedron);
            P_halfedge_iterator hd2 = next(*hd1, polyhedron);
            P_halfedge_iterator hd3 = next(*hd2, polyhedron);
            P_vertex_iterator v1 = source(*hd1, polyhedron);
            P_vertex_iterator v2 = target(*hd1, polyhedron);
            P_vertex_iterator v3 = target(*hd2, polyhedron);
            CGAL::Point_3<CGAL::Epick> ap = (*v1)->point();
            clPoint clP1(ap.x(), ap.y(), ap.z());
            ap = (*v2)->point();
            clPoint clP2(ap.x(), ap.y(), ap.z());
            ap = (*v3)->point();
            clPoint clP3(ap.x(), ap.y(), ap.z());
            scalePoint(clP1, scale);
            scalePoint(clP2, scale);
            scalePoint(clP3, scale);
            gp_Pnt p1 = toOccPoint(clP1);
            gp_Pnt p2 = toOccPoint(clP2);
            gp_Pnt p3 = toOccPoint(clP3);
            //有面网格构建树
            // constructs AABB tree(只有一个三角形)
            clMesh smTemp;
            SM_vertex_descriptor u = smTemp.add_vertex(clP1);
            SM_vertex_descriptor v = smTemp.add_vertex(clP2);
            SM_vertex_descriptor w = smTemp.add_vertex(clP3);
            smTemp.add_face(u, v, w);
            clMeshTree tree(faces(smTemp).first, faces(smTemp).second, smTemp);
            //创建相交平面
            // counts #intersections
            cgalPlane pln_query(toCgalPoint(pntOnTri), toCgalVector(plnNormalDir));
			//获取所有相交
            bool hasDefineTriCS = false;
            gp_Pnt triPlnInterPnt1, triPlnInterPnt2;
            std::list<Plane_MeshIntersection> intersections;
            tree.all_intersections(pln_query, std::back_inserter(intersections));
            for (const Plane_MeshIntersection& inter : intersections)
            {
              // gets intersection object
              const cgalSegment* seg = boost::get<cgalSegment>(&(inter->first));
              if (seg) {
                triPlnInterPnt1 = toOccPoint(seg->start());
                triPlnInterPnt2 = toOccPoint(seg->end());
                hasDefineTriCS = true;
                break;
              }
            }
			
            if (!hasDefineTriCS)
              break;
            gp_Dir aDir(gp_Vec(triPlnInterPnt1, triPlnInterPnt2));
            if (aDir.Angle(interLineDir) > M_PI / 2)
            {
              aDir.Reverse();
              EXCHANGE2OBJ(gp_Pnt, triPlnInterPnt1, triPlnInterPnt2);
            }
            interLineDir = aDir;
            //贴
            Standard_Real len = pntOnTri.Distance(triPlnInterPnt2);
            if (len < length)
            {
              length -= len;
              canContinue = true;
            }
            else
            {
              gp_Pnt addedPnt = pntOnTri.Translated(gp_Vec(triPlnInterPnt1, triPlnInterPnt2).Normalized()*length);
              wrapPnts[ci] = gp_Ax1(addedPnt, dirOnTri);
              wrapPntOnFaces[ci] = face_it;
			  length = 0.0;
              break;//已贴完，退出while，继续一个wire的贴上
            }
			
            //下一个三角形（一个或多个）
            //此处为了简化，如果正好在点上，则偏移一点点让其在边上。
            //因为如果在点上计算的话比较复杂，要找到该点处的所有面，并尝试往每个面上贴，如后面注释
            //在第1个顶点上
            if (triPlnInterPnt2.IsEqual(p1, Precision::Confusion()))
            {
              triPlnInterPnt2.Translate(gp_Vec(p1, p2).Normalized()*0.0001);
            }
            //在第2个顶点上
            else if (triPlnInterPnt2.IsEqual(p2, Precision::Confusion()))
            {
              triPlnInterPnt2.Translate(gp_Vec(p2, p3).Normalized()*0.0001);
            }
            //在第3个顶点上
            else if (triPlnInterPnt2.IsEqual(p3, Precision::Confusion()))
            {
              triPlnInterPnt2.Translate(gp_Vec(p3, p1).Normalized()*0.0001);
            }
            //
            //如果两个边在一个角处非常近，可能点会在多个边上，要找出真正的哪一个
            //当P0判断在两个边上时，利用直线距离最近，可进一步判断是在哪个边上。
            //如：P0P1+P0P3<P0P2+P0P1,可以判断P0在P1P3上。
            //       P1
            //       /\
            //      /  \
            //     /    * P0 (triPlnInterPnt2)
            //    /      \
            //   /________\
            //  P2         p3
            //
            bool b1 = MOCC_ComFuns_Math::isOnLine(triPlnInterPnt2, p1, p2);
            bool b2 = MOCC_ComFuns_Math::isOnLine(triPlnInterPnt2, p2, p3);
            bool b3 = MOCC_ComFuns_Math::isOnLine(triPlnInterPnt2, p3, p1);
            double d1 = triPlnInterPnt2.Distance(p1);
            double d2 = triPlnInterPnt2.Distance(p2);
            double d3 = triPlnInterPnt2.Distance(p3);
            double d12 = p1.Distance(p2);
            double d13 = p1.Distance(p3);
            double d23 = p2.Distance(p3);
            //可能在p1-p2上或p2-p3上
            if (b1 && b2)
            {
              double delta1 = Abs(d1+d2-d12);
              double delta2 = Abs(d2+d3-d23);
              //在p1-p2上
              if (delta1 < delta2)
                b2 = false;
              //在p2 - p3上
              else
                b1 = false;
            }
            //可能在p1-p2上或p1-p3上
            if (b1 && b3)
            {
              double delta1 = Abs(d1 + d2 - d12);
              double delta2 = Abs(d1 + d3 - d13);
              //在p1-p2上
              if (delta1 < delta2)
                b3 = false;
              //在p1 - p3上
              else
                b1 = false;
            }
            //可能在p2-p3上或p1-p3上
            if (b2 && b3)
            {
              double delta1 = Abs(d2 + d3 - d23);
              double delta2 = Abs(d1 + d3 - d13);
              //在p2-p3上
              if (delta1 < delta2)
                b3 = false;
              //在p1 - p3上
              else
                b2 = false;
            }
            //在第1个边上
            if (b1)
            {
              canContinue = toNextFace(polyhedron, hd1, face_it, triPlnInterPnt2, p1, p2, pntOnTri, dirOnTri, interLineDir, plnNormalDir,scale);
            }
            //在第2个边上
            else if (b2)
            {
              canContinue = toNextFace(polyhedron, hd2, face_it, triPlnInterPnt2, p2, p3, pntOnTri, dirOnTri, interLineDir, plnNormalDir, scale);
            }
            //在第3个边上
            else if (b3)
            {
              canContinue = toNextFace(polyhedron, hd3, face_it, triPlnInterPnt2, p3, p1, pntOnTri, dirOnTri, interLineDir, plnNormalDir, scale);
            }
            if (!canContinue)
            {
              bool why = false;
            }
		  }
	   }

        //第三步骤：包覆点分组，每一组代表一个曲线
        /************************************************************************************/
        std::vector< std::vector<gp_Ax1>> allWrapPnts;
        allWrapPnts.resize(count);
        for (int ci = 0; ci < count - 1; ci++)
        {
          const gp_Ax1& curPnt = wrapPnts[ci];
          const gp_Ax1& nextPnt = wrapPnts[ci + 1];
          const P_face_iterator& curFace_it = wrapPntOnFaces[ci];
          const P_face_iterator& nextFace_it = wrapPntOnFaces[ci + 1];
          if (curFace_it.base() != NULL && nextFace_it.base() != NULL)
          {
              allWrapPnts[ci].push_back(curPnt);
              allWrapPnts[ci].push_back(nextPnt);
          }
        }
		
		//分成多个wire组，以没有包覆点的位置为分割处
        wrapPnts.clear();
        for (int ci = 0; ci < count; ci++)
        {
          if (allWrapPnts[ci].size() == 0)
          {
            if (wrapPnts.size() > 0)
            {
              wirePnts.push_back(wrapPnts);
              wrapPnts.clear();
            }
          }
          else
          {
            for (int pi = 0; pi < (int)allWrapPnts[ci].size(); pi++)
            {
              const gp_Ax1& ax1 = allWrapPnts[ci][pi];
              if (wrapPnts.size() == 0 || !wrapPnts.back().Location().IsEqual(ax1.Location(), Precision::Confusion()))
                wrapPnts.push_back(ax1);
            }
          }
        } //第三步结束
    } //第二步结束
}