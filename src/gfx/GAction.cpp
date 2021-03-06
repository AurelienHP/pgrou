#include <iostream>
#include <QMatrix>
#include <QPointF>
#include <QVector>
#include <QColor>
#include <Qt>
#include <QVector2D>
#include "GAction.h"
#include <QtCore/qmath.h>


GAction::GAction(ActionPtr a, PHScene* sc) : scene(sc), action(a) {
    display = new QGraphicsItemGroup();

    initContactPoints();

    hitLine= new QGraphicsPathItem(createHitPath(),display);
    if((targetPoint->x()==resultPoint->x())&&(targetPoint->y()==resultPoint->y())) {
	QPen pen;
        pen.setWidth(2);
        pen.setBrush(Qt::red);
	hitLine->setPen(pen);
    }else{
        QPen pen;
        pen.setWidth(1);
	pen.setBrush(Qt::black);
	hitLine->setPen(pen);
    }

    boundArc = new QGraphicsPathItem(createBoundPath(),display);
    boundArc->setPen(QPen(Qt::DashLine));
    
}

void GAction::update() {

    updateContactPoints();

    boundArc->setPath(createBoundPath());
    hitLine->setPath(createHitPath());
    if((targetPoint->x()==resultPoint->x())&&(targetPoint->y()==resultPoint->y())) {
	QPen pen;
        pen.setWidth(2);
        pen.setBrush(Qt::red);
	hitLine->setPen(pen);
    }else{
        QPen pen;
        pen.setWidth(1);
        pen.setBrush(Qt::black);
	hitLine->setPen(pen);
    }

}

GAction::GAction(){
}

// Important for the destructor : do not delete scene, it's owned by a shared pointer in PH.h
GAction::~GAction() {
    delete display;
}

void GAction::initContactPoints(){
    GSortPtr sourceSort = scene->getGSort(action->getSource()->getSort()->getName());
    GSortPtr targetSort = scene->getGSort(action->getTarget()->getSort()->getName());

    if(sourceSort->getSimpleDisplay()!=1||targetSort->getSimpleDisplay()!=1){
	initPointsInDetailledModel();
    }else {
	initPointsInSimpleModele();
    }
}

void GAction::updateContactPoints(){

    GSortPtr sourceSort = scene->getGSort(action->getSource()->getSort()->getName());
    GSortPtr targetSort = scene->getGSort(action->getTarget()->getSort()->getName());

    if(sourceSort->getSimpleDisplay()!=1||targetSort->getSimpleDisplay()!=1){
	updatePointsInDetailledModel();
    }else{
	updatePointsInSimpleModel();
    }
}

void GAction::initPointsInSimpleModele(){
    	GSortPtr sourceSort = scene->getGSort(action->getSource()->getSort()->getName());
    	GSortPtr targetSort = scene->getGSort(action->getTarget()->getSort()->getName());

	sourcePoint = new QPointF(sourceSort->getCenterPoint());	
	targetPoint = new QPointF(targetSort->getCenterPoint());
	resultPoint = new QPointF(targetSort->getCenterPoint());
}

void GAction::initPointsInDetailledModel(){
	isAutoHit() ? initPointsAutoHit() : initPointsNormalHit();
}

void GAction::updatePointsInSimpleModel(){

    	GSortPtr sourceSort = scene->getGSort(action->getSource()->getSort()->getName());
    	GSortPtr targetSort = scene->getGSort(action->getTarget()->getSort()->getName());

	sourcePoint->setX(sourceSort->getCenterPoint().x());
	sourcePoint->setY(sourceSort->getCenterPoint().y());	
	targetPoint->setX(targetSort->getCenterPoint().x());
	targetPoint->setY(targetSort->getCenterPoint().y());
	resultPoint->setX(targetSort->getCenterPoint().x());
	resultPoint->setY(targetSort->getCenterPoint().y());

}

void GAction::updatePointsInDetailledModel(){
	isAutoHit() ? updatePointsAutoHit() : updatePointsNormalHit();
}

bool GAction::isAutoHit(){
	GProcessPtr source = getSource();
	GProcessPtr target = getTarget();

	return source==target;
}

void GAction::initPointsNormalHit(){
	GProcessPtr source = getSource();
	GProcessPtr target = getTarget();
	GProcessPtr result = getResult();

	QVector2D* hitVector = new QVector2D(*(target->getCenterPoint()) - *(source->getCenterPoint()));
	hitVector->normalize();

	QSizeF* sizeSource = source->getSizeEllipse();
	QSizeF* sizeTarget = source->getSizeEllipse();


	sourcePoint = new QPointF(sizeSource->width()*hitVector->x()/2.0 + source->getCenterPoint()->x(),sizeSource->height()*hitVector->y()/2 + source->getCenterPoint()->y());

	targetPoint = new QPointF(-sizeTarget->width()*hitVector->x()/2.0 + target->getCenterPoint()->x(),-sizeTarget->height()*hitVector->y()/2 + target->getCenterPoint()->y());

	resultPoint = new QPointF(-sizeTarget->width()*hitVector->x()/2.0 + result->getCenterPoint()->x(),sizeTarget->height()*hitVector->y()/2 + result->getCenterPoint()->y());

}

void GAction::updatePointsNormalHit(){
	GProcessPtr source = getSource();
	GProcessPtr target = getTarget();
	GProcessPtr result = getResult();

        QVector2D* hitVector = new QVector2D(*(target->getCenterPoint()) - *(source->getCenterPoint()));
        hitVector->normalize();

        QSizeF* sizeSource = source->getSizeEllipse();
        QSizeF* sizeTarget = target->getSizeEllipse();

        sourcePoint->setX(sizeSource->width()*hitVector->x()/2.0 + source->getCenterPoint()->x());
        sourcePoint->setY(sizeSource->height()*hitVector->y()/2.0 + source->getCenterPoint()->y());
        targetPoint->setX(-sizeTarget->width()*hitVector->x()/2.0 + target->getCenterPoint()->x());
        targetPoint->setY(-sizeTarget->height()*hitVector->y()/2.0 + target->getCenterPoint()->y());
        resultPoint->setX(-sizeTarget->width()*hitVector->x()/2.0 + result->getCenterPoint()->x());
        resultPoint->setY(-sizeTarget->height()*hitVector->y()/2.0 + result->getCenterPoint()->y());
}

void GAction::initPointsAutoHit(){
	GProcessPtr source = getSource();
	GProcessPtr target = getTarget();
	GProcessPtr result = getResult();

	sourcePoint = new QPointF(source->getCenterPoint()->x(), source->getCenterPoint()->y() +(GProcess::sizeDefault)/2.0);
	targetPoint = new QPointF(GProcess::sizeDefault/2.0 + source->getCenterPoint()->x(), source->getCenterPoint()->y());
	resultPoint = new QPointF(GProcess::sizeDefault/2.0 + result->getCenterPoint()->x(),result->getCenterPoint()->y());

}

void GAction::updatePointsAutoHit(){
	GProcessPtr source = getSource();
	GProcessPtr target = getTarget();
	GProcessPtr result = getResult();

        sourcePoint->setX(source->getCenterPoint()->x() + 0*(GProcess::sizeDefault)/2.0);
        sourcePoint->setY(source->getCenterPoint()->y() + qSin(qAcos(0))*(GProcess::sizeDefault)/2.0);
        targetPoint->setX(GProcess::sizeDefault/2 + source->getCenterPoint()->x());
        targetPoint->setY(source->getCenterPoint()->y());
        resultPoint->setX(GProcess::sizeDefault/2 + result->getCenterPoint()->x());
        resultPoint->setY(result->getCenterPoint()->y());
}

// Create actions paths
//Hit Part
QPainterPath GAction::createHitPath(){
    GProcessPtr source = getSource();
    GProcessPtr target = getTarget();
    GProcessPtr result = getResult();

    qreal rectCornerX;
    qreal rectCornerY;
    qreal widthRect;
    qreal heightRect;
    qreal sweepAngle;
    qreal startAngle;

    QPainterPath hitPath(*sourcePoint);

    if(source!=target){
       hitPath.lineTo(*targetPoint);
    }else{
        rectCornerY = source->getCenterPoint()->y();
        rectCornerX = sourcePoint->x();
        heightRect = (sourcePoint->y() - targetPoint->y())*2;
        startAngle = 180 ;
        sweepAngle = 270;
        widthRect = (targetPoint->x() - sourcePoint->x())*2;
        hitPath.arcTo(QRectF(rectCornerX,rectCornerY,widthRect,heightRect),startAngle,sweepAngle);
    }
    	
    //if((targetPoint->x()!=resultPoint->x())||(targetPoint->y()!=resultPoint->y())) 
	hitPath.addPolygon(makeArrowHead(hitPath));


    return hitPath;
}
// Bound Part
QPainterPath GAction::createBoundPath(){
    QPainterPath boundPath(*targetPoint);

    if((targetPoint->x()!=resultPoint->x())||(targetPoint->y()!=resultPoint->y())){
	qreal rectCornerX;
	qreal rectCornerY;
	qreal widthRect;
	qreal heightRect;
	qreal sweepAngle;
	qreal startAngle;
	int invertSweep;

	if(dynamic_cast<GSort*>(getTarget()->getDisplayItem()->parentItem())->isVertical()){

		if(targetPoint->y()<resultPoint->y()){
			rectCornerY = targetPoint->y();
			heightRect = resultPoint->y()-targetPoint->y();
			startAngle = 90;
			invertSweep = 1;
		} else{
			rectCornerY = resultPoint->y();
			heightRect = targetPoint->y()-resultPoint->y();
			startAngle = -90;
			invertSweep = -1;
		}

		if(resultPoint->x() < getResult()->getCenterPoint()->x()){
			sweepAngle = 180*invertSweep;
		}else{
			sweepAngle = -180*invertSweep;
		}

		rectCornerX = resultPoint->x()- (GProcess::sizeDefault)/2.0;
		widthRect = GProcess::sizeDefault;
	}
	else{
		if(targetPoint->x()<resultPoint->x()){ //target point à gauche de resultpoint
		    rectCornerX = targetPoint->x();
		    widthRect = resultPoint->x() - targetPoint->x();
		    startAngle =180;
		    invertSweep = -1;
		}
		else{ //resultpoint à gauche de targetpoint
		    rectCornerX = resultPoint->x();
		    widthRect = targetPoint->x() - resultPoint->x();
		    startAngle =0;
		    invertSweep = 1;
		}

		if(resultPoint->y()<getResult()->getCenterPoint()->y()){ //resultpoint au-dessus du centre du process
		    sweepAngle = 180*invertSweep;
		    //startAngle = 0;
		}else{ //resultpoint en-dessus du centre du process
		    sweepAngle = -180*invertSweep;
		    //startAngle = 180;
		}
		rectCornerY = resultPoint->y() - (GProcess::sizeDefault)/2.0;
		heightRect = GProcess::sizeDefault;
	}


	boundPath.arcTo(QRectF(rectCornerX,rectCornerY,widthRect,heightRect),startAngle,sweepAngle);

	boundPath.addPolygon(makeArrowHead(boundPath));
    }

    return boundPath;
}

// draw an arrowhead
QPolygonF GAction::makeArrowHead(QPainterPath path) {

    // arrow pointing to the right
    QPointF p = path.pointAtPercent(1);
    QPointF* q = new QPointF(p.x() - 8, p.y() - 5);
    QPointF* r = new QPointF(p.x() - 8, p.y() + 5);
    QPointF* s = new QPointF(p.x() - 1, p.y());
    QPointF* t = new QPointF(p.x() - 7.5, p.y() + 4);
    QPointF* u = new QPointF(p.x() - 7.5, p.y() - 4);
    QPolygonF polygon;
    polygon.push_back(p);
    polygon.push_back(*q);
    polygon.push_back(*u);
    polygon.push_back(*s);
    polygon.push_back(*t);
    polygon.push_back(*r);

    // rotate arrow
    QMatrix matrix;
    matrix.translate(p.x(), p.y());
    if(sourcePoint!=targetPoint)
    {
        matrix.rotate(-path.angleAtPercent(1));
    }else{
        matrix.rotate(-path.angleAtPercent(0.98));
    }
    matrix.translate(-p.x(), -p.y());
    polygon = matrix.map(polygon);

    // turn into QGraphicsPolygonItem
    return polygon;
}

// getters

QGraphicsItem* GAction::getDisplayItem (void) {
    return display;
}

ActionPtr GAction::getAction() {
    return action;
}

GProcessPtr GAction::getSource() {
    return action->getSource()->getGProcess();
}

GProcessPtr GAction::getTarget() {
    return action->getTarget()->getGProcess();
}

GProcessPtr GAction::getResult() {
    return action->getResult()->getGProcess();
}
