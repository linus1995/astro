// Programm zur Bearbeitung von Astrofotos

#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <QVector>
#include <QDebug>
#include <QPainter>
#include <QFileDialog>
#include <QPushButton>
#include <iostream>
#include <QSlider>
#include <QLabel>
#include <QFont>
//#include <Eigen/Dense>


class PhotoViewer : public QGraphicsView {

Q_OBJECT

public:
     explicit PhotoViewer(QWidget* parent = nullptr) {

        // Größe des Grafikelements noch an die Bildgröße anpassen?
        //QGraphicsPixmapItem* pixmapItem = new QGraphicsPixmapItem();
        //scene = new QGraphicsScene();
        //scene->addItem(pixmapItem);
        //setScene(scene);


        // Button für Bildauswahl
        QPushButton* openImageButton = new QPushButton("Bild öffnen", this);
        connect(openImageButton, &QPushButton::clicked, this, PhotoViewer::selectImage);

        // Button positionieren
        openImageButton->setGeometry(10+120, height()+30 , 80, 30);

        // Button für Bildaddition
        QPushButton* AddImageButton = new QPushButton("Add Picture", this);
        connect(AddImageButton, &QPushButton::clicked, this, &PhotoViewer::selectImageandAdd);

        // Button positionieren
        AddImageButton->setGeometry(240+120, height()+30 , 80, 30);

        // Button für Hot-Pixel-Entfernung erstellen
        QPushButton* hotPixelButton = new QPushButton("Hot Pixel entfernen", this);
        connect(hotPixelButton, &QPushButton::clicked, this, &PhotoViewer::removeHotPixels);
        hotPixelButton->setGeometry(480+120, height()+30 , 150, 30);


        // Button für Auswahl Punkte/Referenzstern
        QPushButton* PointButton = new QPushButton("Referenzstern", this);
        PointButton->setGeometry(120+120, height()+30 , 80, 30);


        //connect(PointButton, &QPushButton::clicked, this,        );    // Slot für Punkt-Auswahl verbinden



        // Button zum Speichern
        QPushButton* saveButton = new QPushButton("Speichern", this);
        connect(saveButton, &QPushButton::clicked, this, &PhotoViewer::saveImage);

        saveButton->setGeometry(360+120, height()+30  , 80, 30);


        // Button für Sternanpassung
        QPushButton* StarButton = new QPushButton("Sternanpassung", this);
        //connect(StarButton, &QPushButton::clicked, this, &PhotoViewer::GaussianFit);

        StarButton->setGeometry(480+120, height()+140  , 150, 30);


        // Helligkeitsschieberegler erstellen
        QSlider* brightnessSlider = new QSlider(Qt::Horizontal, this);
        brightnessSlider->setGeometry(10, height() + 150, 420, 20);
        brightnessSlider->setMinimum(-100);  // Minimale Helligkeit
        brightnessSlider->setMaximum(100);   // Maximale Helligkeit
        connect(brightnessSlider, &QSlider::valueChanged, this, &PhotoViewer::adjustBrightness);

        QLabel* brightnessLabel = new QLabel("Helligkeit", this);
        brightnessLabel->setGeometry(10, height() + 175, 100, 20);


        QFont font = brightnessLabel->font();
        font.setPointSize(14);
        brightnessLabel->setFont(font);

        // contrast slider erstellen
        //QSlider* contrastSlider = new QSlider(Qt::Horizontal, this);
        //contrastSlider->setGeometry(10, height() + 160, 420, 20);
        //contrastSlider->setMinimum(-100);  // Minimaler Kontrast
        //contrastSlider->setMaximum(100);   // Maximaler Kontrast


        // Größe der GUI
        resize(940, 720);

    }




private:

    QPoint maxIntensityPixel;
    double Factor = 0.12;
    QImage image;
    QGraphicsScene* scene;
    QVector<QPoint> selectedPoints;
    QGraphicsPixmapItem* firstItem;


    void selectImage() {
        // Öffne einen Datei-Dialog für Bilddateien
        QString initialPath = "Testfotos_Ringnebel";
        QString imagePath = QFileDialog::getOpenFileName(this, tr("Bild auswählen"), initialPath, tr("Bilddateien (*.png *.jpg *.bmp)"));

            // Wenn Datei ausgewählt
            if (! imagePath.isEmpty()) {

            // Foto von der ausgewählten Datei (Pfad) zu laden
            if (image.load(imagePath)) {

                // Prüfen, ob scene schon existiert, falls nicht erstellen
                if (! scene) {
                    scene = new QGraphicsScene();
                    setScene(scene);
                }

                // Pixmap aus Image
                QGraphicsPixmapItem* pixmapItem = new QGraphicsPixmapItem(QPixmap::fromImage(image));

                // Szene und Grafikelement erstellen
                scene->clear();
                scene->addItem(pixmapItem);

                // erste Element der Szene wird gespeichert
                firstItem = pixmapItem;

                // QGraphicsView skalieren mit Factor
                qreal scaleFactor = Factor;
                setTransform(QTransform().scale(scaleFactor, scaleFactor));




            } else {
                qDebug() << "Fehler beim Laden --> Fallback.";
                    setFallbackImage();
            }

        } else {
            // Datei-Dialog abgebrochen oder keine Datei ausgewählt
            qDebug() << "Bildauswahl --> Fallback.";
            setFallbackImage();
        }
    }

    // ToDo Fallback-Methode zum Setzen des Bildes direkt im Code
    void setFallbackImage() {
    }




protected:
    void mousePressEvent(QMouseEvent* event) override {

        if (scene) {

            // Punkte auswählen
            selectedPoints.reserve(4);

            if (event->button() == Qt::LeftButton) { // Wenn Maus (mit links) gedrückt
                QPoint point = mapToScene(event->pos()).toPoint(); // Position in Koodinatenpunkt umwandeln
                selectedPoints.push_back(point*Factor);

                // Roter Punkt auf dem Foto zeichnen
                scene->addEllipse(point.x() - 5/Factor, point.y() - 5/Factor, 10/Factor, 10/Factor, QPen(Qt::red), QBrush(Qt::red));

                // Wenn vier Punkte ausgewählt, dann Positionen speichern
                if (selectedPoints.size() == 4) {

                    for (const auto& point : selectedPoints) {
                        qDebug() << "Punkt: (" << point.x() << ", " << point.y() << ")";

                    }
                QPoint maxIntensityPixel1 = findMaxIntensityPixelAndMark(firstItem->pixmap().toImage());
                }
            }
        }
    }


    QPoint findMaxIntensityPixelAndMark(const QImage& image) {

        // Bereich des Vierecks
        QRect boundingRect = QRect(selectedPoints[0], selectedPoints[2]);

        int maxIntensity = 0;
        QPoint maxPixel;

        for (int y = boundingRect.top(); y < boundingRect.bottom(); ++y) {
            for (int x = boundingRect.left(); x < boundingRect.right(); ++x) {
            QPoint scaledPoint(x / Factor, y / Factor);  // Anpassung Skalierung
            QRgb pixel = image.pixel(scaledPoint);

            int intensity = qGray(pixel);

            if (intensity > maxIntensity) {
                    maxIntensity = intensity;
                    maxPixel = scaledPoint;
            }
            }
        }

        // Koordinaten des Pixels mit der höchsten Intensität
        qDebug() << " Max Intensity Pixel: (" << maxPixel.x() << ", " << maxPixel.y() << ")";

        // blauer Punkt
        scene->addEllipse(maxPixel.x() - 3/Factor, maxPixel.y() - 3/Factor, 6/Factor, 6/Factor, QPen(Qt::blue), QBrush(Qt::blue));

        return maxPixel;
    }




    void removeHotPixels() {
        if (!firstItem) {
            qDebug() << "Kein Bild geladen.";
            return;
        }

        QImage originalImage = firstItem->pixmap().toImage();
        QImage resultImage = originalImage;

        // Iteriere über jedes Pixel des Bildes
        for (int y = 0; y < originalImage.height(); ++y) {
            for (int x = 0; x < originalImage.width(); ++x) {
                QRgb currentPixel = originalImage.pixel(x, y);

                // Überprüfe, ob die Intensität das a-Fache der vorherigen Intensität überschreitet
                if (x > 0 && y > 0) {
                    QRgb previousPixel = originalImage.pixel(x - 1, y - 1);

                    int currentIntensity = qGray(currentPixel);
                    int previousIntensity = qGray(previousPixel);

                    // Setze das Pixel auf die Intensität des vorherigen Pixels, wenn Bedingung erfüllt ist
                    if (currentIntensity > 1.2 * previousIntensity) {
                        resultImage.setPixel(x, y, previousPixel);
                    }
                }
            }
        }

        // Zeige das Ergebnisbild in der Szene an
        firstItem->setPixmap(QPixmap::fromImage(resultImage));
        scene->update();
        qDebug() << "Hot Pixel entfernt";
    }




    void selectImageandAdd() {

        // Öffne den Dateiexplorer für die Bildauswahl
        QString imagePath = QFileDialog::getOpenFileName(this, tr("Bild auswählen"), "", tr("Bilddateien (*.png *.jpg *.bmp)"));

            // Überprüfe, ob der Benutzer eine Datei ausgewählt hat
            if (! imagePath.isEmpty()) {
            // Foto von der ausgewählten Datei (Pfad) laden
            QImage imageToAdd;
            if (imageToAdd.load(imagePath)) {

                // Füge das Bild dem ersten Bild hinzu
                addImage(imageToAdd);

            } else {
                qDebug() << "Fehler beim Laden --> Fallback.";
                    setFallbackImage();
            }
        } else {
            // Datei-Dialog abgebrochen / keine Datei ausgewählt
            qDebug() << "abgebrochen --> Fallback.";
            setFallbackImage();
        }
    }




    void addImage(const QImage& imageToAdd) {
        // Überprüfen, ob das erste Element der Szene ein QGraphicsPixmapItem ist
        QGraphicsPixmapItem* pixmapItem = dynamic_cast<QGraphicsPixmapItem*>(firstItem);

        if (pixmapItem) {
            // Größe des Grafikelements an die Bildgröße anpassen
            QImage baseImage = pixmapItem->pixmap().toImage();



            QPoint maxIntensityPixel1 = findMaxIntensityPixelAndMark(firstItem->pixmap().toImage());
            QPoint maxIntensityPixel2 = findMaxIntensityPixelAndMark(imageToAdd);

            // Direktes Speichern der Koordinaten in Variablen
            int x_2 = maxIntensityPixel2.x();
            int y_2 = maxIntensityPixel2.y();

            int x_1 = maxIntensityPixel1.x();
            int y_1 = maxIntensityPixel1.y();
            std::cout <<  x_2-x_1 ;
            // Zuschneiden erstes Bild
            int x1;  // von links


            x1 = 0;
            if (x_2-x_1 < 0){
                    x1= -(x_2-x_1);
            }



            int x2 = 0;
            if (x_2-x_1 > 0){
                  x2= x_2-x_1;
            }


            int y1 = 0;
            if (y_2-y_1 < 0) {
                  y1 = -(y_2-y_1);  // von oben
            }



            int  y2 = 0 ;   // von unten
            if (y_2-y_1 > 0) {
                  y2 = y_2-y_1;
            }


            baseImage = cropImage(baseImage, x1, y1, x2, y2);


            // Zuschneiden zweites Bild
            x1 = 0;
            if (x_2-x_1 > 0){
                  x1= x_2-x_1;
            }  // von links



            x2 = 0;
            if (x_2-x_1 < 0){
                  x2 = -(x_2-x_1);
            }


            y1 = 0;
            if (y_2-y_1 > 0) {
                  y1 = y_2-y_1;
            }



            y2 = 0 ;
            if (y_2-y_1 < 0) {
                  y2 = -(y_2-y_1);  // von oben
            }


            QImage croppedImageToAdd = cropImage(imageToAdd, x1, y1, x2, y2);

            // sicher stellen, dass beide Bilder die gleiche Größe haben
            QSize targetSize = baseImage.size().boundedTo(croppedImageToAdd.size());
            baseImage = baseImage.scaled(targetSize, Qt::KeepAspectRatio);
            croppedImageToAdd = croppedImageToAdd.scaled(targetSize, Qt::KeepAspectRatio);

            // Iteriere über die Pixel beider Bilder und addiere die Werte
            for (int y = 0; y < targetSize.height(); ++y) {
                for (int x = 0; x < targetSize.width(); ++x) {
                    QRgb basePixel = baseImage.pixel(x, y);
                    QRgb addedPixel = croppedImageToAdd.pixel(x, y);

                    int newRed = qRed(basePixel) + qRed(addedPixel);
                    int newGreen = qGreen(basePixel) + qGreen(addedPixel);
                    int newBlue = qBlue(basePixel) + qBlue(addedPixel);

                    baseImage.setPixel(x, y, qRgb(newRed, newGreen, newBlue));
                }
            }

            // Setze die Größe des Bildes auf die gleiche Größe wie das Originalbild
            QPixmap updatedPixmap = QPixmap::fromImage(baseImage).scaled(
                pixmapItem->pixmap().size(),
                Qt::KeepAspectRatio
                );

            pixmapItem->setPixmap(updatedPixmap);
        }

        // Szene aktualisieren
        scene->update();
    }



    QImage cropImage(const QImage& originalImage, int x1, int y1, int x2, int y2) {
        // Berechne die neuen Abmessungen des zugeschnittenen Bildes
        int newWidth = originalImage.width() - x1 - x2;
        int newHeight = originalImage.height() - y1 - y2;

        // Überprüfe, ob die neuen Abmessungen positiv sind
        if (newWidth <= 0 || newHeight <= 0) {
            // Unerwartete Größe, gib das Originalbild zurück
            return originalImage;
        }

        // Zuschneiden des Bildes
        QImage croppedImage = originalImage.copy(x1, y1, newWidth, newHeight);

        return croppedImage;
    }



    void adjustBrightness(int value) {

        if (!firstItem) {
            qDebug() << "Kein Bild geladen.";
            return;
        }

        // Wertebereich des Sliders auf [-100, 100] in Faktor zwischen [0.0, 2.0] umrechnen
        qreal brightnessFactor = 1.0 + value / 100.0;

        // Originalbild kopieren
        QImage originalImage = firstItem->pixmap().toImage();
        QImage resultImage = originalImage;

        // Iteriere über jedes Pixel des Bildes
        for (int y = 0; y < originalImage.height(); ++y) {
            for (int x = 0; x < originalImage.width(); ++x) {
                QRgb pixel = originalImage.pixel(x, y);

                // Helligkeit anpassen
                int newRed = qBound(0, static_cast<int>(qRed(pixel) * brightnessFactor), 255);
                int newGreen = qBound(0, static_cast<int>(qGreen(pixel) * brightnessFactor), 255);
                int newBlue = qBound(0, static_cast<int>(qBlue(pixel) * brightnessFactor), 255);

                resultImage.setPixel(x, y, qRgb(newRed, newGreen, newBlue));
            }
        }

        // Ergebnisbild in der Szene anzeigen
        firstItem->setPixmap(QPixmap::fromImage(resultImage));
        scene->update();
    }



    void saveImage() {
        // Überprüfen, ob das erste Element der Szene ein QGraphicsPixmapItem ist
        QGraphicsPixmapItem* pixmapItem = dynamic_cast<QGraphicsPixmapItem*>(firstItem);

        if (pixmapItem) {
            // Holen Sie sich das Bild aus dem Grafikelement
            QImage imageToSave = pixmapItem->pixmap().toImage();

            // Öffnen des Dateiexplorer für den Speicherort
            QString savePath = QFileDialog::getSaveFileName(this, tr("Speichern als"), "", tr("Bilddateien (*.png *.jpg *.bmp)"));

            // Überprüfen Sie, ob der Benutzer einen Speicherort ausgewählt hat
            if (!savePath.isEmpty()) {
                // Speichern des Bildes am ausgewählten Speicherort
                imageToSave.save(savePath);
            }
        }
    }




//    void GaussianFit() {


//        if (!firstItem) {
//            qDebug() << "Kein Bild geladen.";
//            return;
//        }


//        Eigen::MatrixXd intensityMatrix;

//        // Bereich des ausgewählten Rechtecks
//        QRect boundingRect = QRect(selectedPoints[0], selectedPoints[2]);

//        // Überprüfen, ob das erste Element der Szene ein QGraphicsPixmapItem ist
//        QGraphicsPixmapItem* pixmapItem = dynamic_cast<QGraphicsPixmapItem*>(firstItem);

//        qDebug() << "!";
//        if (pixmapItem) {
//            // Bild in eine Matrix konvertieren
//            QImage originalImage = pixmapItem->pixmap().toImage();
//            Eigen::MatrixXd intensityMatrix(boundingRect.height(), boundingRect.width());

//            for (int y = 0; y < boundingRect.height(); ++y) {
//                for (int x = 0; x < boundingRect.width(); ++x) {
//                    QRgb pixel = originalImage.pixel(boundingRect.left() + x, boundingRect.top() + y);
//                    intensityMatrix(y, x) = qGray(pixel);
//                }
//            }
//        }
//        qDebug() << "!!!";


//        // Gauß-Fit durchführen
//        Eigen::VectorXd xVec = Eigen::VectorXd::LinSpaced(boundingRect.width(), boundingRect.left(), boundingRect.right());
//        Eigen::VectorXd yVec = Eigen::VectorXd::LinSpaced(boundingRect.height(), boundingRect.top(), boundingRect.bottom());

//        Eigen::VectorXd xGrid = xVec.replicate(boundingRect.height(), 1);
//        Eigen::VectorXd yGrid = yVec.replicate(1, boundingRect.width());

//        Eigen::VectorXd xData = xGrid.reshaped(boundingRect.height() * boundingRect.width(), 1);
//        Eigen::VectorXd yData = yGrid.reshaped(boundingRect.height() * boundingRect.width(), 1);
//        Eigen::VectorXd intensityData = Eigen::Map<Eigen::VectorXd>(intensityMatrix.data(), boundingRect.height() * boundingRect.width());


//        Eigen::VectorXd ones = Eigen::VectorXd::Ones(xData.size());
//        Eigen::MatrixXd A(xData.size(), 6);
//        A << ones, xData, yData, xData.array().square(), xData.array() * yData.array(), yData.array().square();

//        Eigen::VectorXd result = (A.transpose() * A).ldlt().solve(A.transpose() * intensityData);


        // Korrektur der Sterne mit Fit Parametern

//    }

};


int main(int argc, char** argv) {

    QApplication app(argc, argv);

    PhotoViewer viewer;
    viewer.show();

    return app.exec();
}


#include "main.moc" //
