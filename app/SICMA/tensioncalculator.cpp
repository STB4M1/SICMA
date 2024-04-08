#include "tensioncalculator.h"
#include <QImage>
#include <QDebug>
#include <cmath> // std::roundを使用する場合

TensionCalculator::TensionCalculator(){}

std::tuple<double, double, double>
    TensionCalculator::calculateTension(
        const QImage& image, double pix_el, double g, double d_rho) {

    int i, j;
    int width, height;
//    int width_half, height_half;
    double gamma=0.0;
    double de=0.0, ds=0.0, S=0.0, H=0.0;
    double a=0.0, B0=0.0, B1=0.0, B2=0.0, B3=0.0, B4=0.0;

    qDebug() << "pix_el = " << pix_el; // mm (Element size)
    qDebug() << "cal_g = " << g;
    qDebug() << "cal_d_rho = " << d_rho;

    // 画像の幅と高さを取得
    width = image.width();
    height = image.height();
//    width_half=int(width*0.5);
//    height_half=int(height*0.5);

    qDebug() << "Image dimensions: " << width << "x" << height;
    unsigned char *image_in01 = (unsigned char*)malloc(sizeof(unsigned char) * width*height);

    // ピクセルデータ用のメモリを確保
    if (image_in01 == nullptr) {
        qDebug() << "Memory allocation for image_in01 failed.";
//        return ;
    }

    // 各ピクセルの輝度値を取得し、配列に格納
    for(j=0; j<height; j++) {
        for (i=0; i<width; i++) {
            QRgb pixel = image.pixel(i, j);
            int luminance = qGray(pixel); // QRgbから輝度値を計算 (グレースケールの時は，このように輝度値を取得できる)
            //int luminance = 0.299 * qRed(pixel) + 0.587 * qGreen(pixel) + 0.114 * qBlue(pixel); // 手動で輝度を計算する場合
            image_in01[width*j + i] = static_cast<unsigned char>(luminance);
//            if(image_in01[width*j+i]<0){qDebug() << "Find < 0 !";}
        }
    }
//    qDebug() << image_in01[width*0+width_half];
//    qDebug() << image_in01[0];
//    qDebug() << image_in01[width*(height-1)+(width-1)];
//    qDebug() << image_in01[width*height_half+500];
//    qDebug() << image_in01[width*height_half+width_half];


    int drop_width[height], bottom_pixel[width];
    int max_drop_width=0, max_bottom_pixel=0;

    for(j=0; j<height; j++){
        int flag = 0; // フラグ変数の初期化 (0は条件がまだ満たされていないことを意味する)
        int left_pixel=0, right_pixel=0;
        drop_width[j]=0.0;
        for(i=0; i<width; i++){
            if(image_in01[width*j+i]==0 && flag == 0){
                left_pixel=i;
                flag = 1; // 条件が満たされたことを示すためにフラグを更新
            }
        }
        flag=0;
        for(i=width-1; i>=0; i--){
            if(image_in01[width*j+i]==0 && flag == 0){
                right_pixel=i;
                flag = 1; // 条件が満たされたことを示すためにフラグを更新
            }
        }
        // qDebug() << "left_pixel=" << left_pixel << ", right_pixel=" << right_pixel << endl;
        drop_width[j]=right_pixel-left_pixel;
        // printf("%d\n", drop_width[j]);
    }
//    qDebug() << QString("%1 mm").arg(drop_width[height-1] * pix_el, 0, 'f', 4);
//    qDebug() << QString("%1 mm").arg(drop_width[height-500] * pix_el, 0, 'f', 4);
//    qDebug() << QString("%1 mm").arg(drop_width[height-1000] * pix_el, 0, 'f', 4);
//    qDebug() << QString("%1 mm").arg(drop_width[height_half] * pix_el, 0, 'f', 4);
//    qDebug() << QString("%1 mm").arg(drop_width[height_half-200] * pix_el, 0, 'f', 4);
//    qDebug() << QString("%1 mm").arg(drop_width[height_half-400] * pix_el, 0, 'f', 4);
//    qDebug() << QString("%1 mm").arg(drop_width[height_half-600] * pix_el, 0, 'f', 4);
//    qDebug() << QString("%1 mm").arg(drop_width[800] * pix_el, 0, 'f', 4);
//    qDebug() << QString("%1 mm").arg(drop_width[0] * pix_el, 0, 'f', 4);

    for(j=0; j<height; j++){
        if(drop_width[j]>max_drop_width){
            max_drop_width=drop_width[j];
        }
    }
    de=max_drop_width*pix_el*0.001;
    qDebug() << "de = " << de << " m";

    for(i=0; i<width; i++){
        bottom_pixel[i]=0;
        int flag = 0; // フラグ変数の初期化 (0は条件がまだ満たされていないことを意味する)
        for(j=0; j<height; j++){ // 画像の左下に原点があるかも？ for(j=height-1; j>=0; j--)だと，常に max_bottom_pixel=heightになってしまう．
            if (image_in01[width*j+i]==0 || flag==0) {
                bottom_pixel[i] = j;
                flag=1;
//                break; // 黒いピクセルを見つけたら、その列の走査を終了
            }
        }
    }

    // bottom_pixel関連のデバッグ出力
//    qDebug() << QString("%1 pixel").arg(bottom_pixel[0]);
//    qDebug() << QString("%1 pixel").arg(bottom_pixel[width / 8]);
//    qDebug() << QString("%1 pixel").arg(bottom_pixel[width / 4]);
//    qDebug() << QString("%1 pixel").arg(bottom_pixel[width_half]);
//    qDebug() << QString("%1 pixel").arg(bottom_pixel[width-600]);
//    qDebug() << QString("%1 pixel").arg(bottom_pixel[width-200]);
//    qDebug() << QString("%1 pixel").arg(bottom_pixel[width-1]);

    for(i=0; i<width; i++){
        if(bottom_pixel[i]>max_bottom_pixel){
            max_bottom_pixel=bottom_pixel[i]; // 新しい最大値で更新
        }
    }
    qDebug() << "max_drop_width_pixels = " << max_drop_width;
    qDebug() << "max_bottom_pixel = " << max_bottom_pixel;
    int  width_pixel = max_bottom_pixel-max_drop_width;
    ds=drop_width[width_pixel]*pix_el * 0.001;
    qDebug() << "end?";
    qDebug() << "ds = " << ds << " m";

    /********************** Calculate S and H ***********************/

    S=ds/de;

    if(S<0.401){
        H=1/S;
    }
    if(0.401<=S && S<0.46){
        a=2.56651; B4=0.32720; B3=0.0; B2=0.97553; B1=0.84059; B0=0.18069;
        H=1/(B4/pow(S, a) + B3*S*S*S - B2*S*S + B1*S - B0);
    }
    if(0.46<=S && S<0.59){
        a=2.59725; B4=0.31968; B3=0.0; B2=0.46898; B1=0.50059; B0=0.13261;
        H=1/(B4/pow(S, a) + B3*S*S*S - B2*S*S + B1*S - B0);
    }
    if(0.59<=S && S<0.68){
        a=2.62435; B4=0.31522; B3=0.0; B2=0.11714; B1=0.15756; B0=0.05285;
        H=1/(B4/pow(S, a) + B3*S*S*S - B2*S*S + B1*S - B0);
    }
    if(0.68<=S && S<0.90){
        a=2.64267; B4=0.31345; B3=0.0; B2=0.09155; B1=0.14701; B0=0.05877;
        H=1/(B4/pow(S, a) + B3*S*S*S - B2*S*S + B1*S - B0);
    }
    if(0.90<=S && S<=1.00){
        a=2.84636; B4=0.30715; B3=-0.69116; B2=-1.08315; B1=-0.18341; B0=0.20970;
        H=1/(B4/pow(S, a) + B3*S*S*S - B2*S*S + B1*S - B0);
    }

    qDebug() << "S = " << S;
    qDebug() << "H = " << H;

    gamma = d_rho*g*(de*de)*1/H;

    qDebug() << "gamma = " << gamma;


    free(image_in01);

    return {gamma, de, ds};
}


