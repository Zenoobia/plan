/*
OpenCV Tutorial Youtube channel: youtube/jackyltle
Author: Jacky Le
Date: 01.Sep.2016
Project: licPlaterec
Desciption: This project support to made a Automation Car/moto plate recognition use OpenCV API.
This source code is free & use for education purpose only
*/

#pragma once
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/ml.hpp>
#include "dirent.h"
#include "feature.h"
#include "jackylib.h"
#include <iostream>
namespace licPlaterec {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace cv;
	using namespace std;
	using namespace cv::ml;
	using namespace jacky_lib;

	Mat srcImg;
	str2char stringTochar;
	mat2picture mat2bmp;

	/// <summary>
	/// Summary for MainForm
	/// </summary>
	public ref class MainForm : public System::Windows::Forms::Form
	{
	public:
		MainForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MainForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::GroupBox^  groupBox1;
	protected:
	private: System::Windows::Forms::PictureBox^  ptbSrc;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::PictureBox^  ptbCha;

	private: System::Windows::Forms::PictureBox^  ptbPlate;

	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::Button^  btnProcess;
	private: System::Windows::Forms::Button^  btnLoad;
	private: System::Windows::Forms::Button^  btnLearn;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  imgSize;


	private: System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->ptbSrc = (gcnew System::Windows::Forms::PictureBox());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->ptbCha = (gcnew System::Windows::Forms::PictureBox());
			this->ptbPlate = (gcnew System::Windows::Forms::PictureBox());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->btnLearn = (gcnew System::Windows::Forms::Button());
			this->btnProcess = (gcnew System::Windows::Forms::Button());
			this->btnLoad = (gcnew System::Windows::Forms::Button());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->imgSize = (gcnew System::Windows::Forms::Label());
			this->groupBox1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ptbSrc))->BeginInit();
			this->groupBox2->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ptbCha))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ptbPlate))->BeginInit();
			this->groupBox3->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->ptbSrc);
			this->groupBox1->Location = System::Drawing::Point(12, 12);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(622, 474);
			this->groupBox1->TabIndex = 0;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Src Image";
			// 
			// ptbSrc
			// 
			this->ptbSrc->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->ptbSrc->Location = System::Drawing::Point(6, 19);
			this->ptbSrc->Name = L"ptbSrc";
			this->ptbSrc->Size = System::Drawing::Size(610, 449);
			this->ptbSrc->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->ptbSrc->TabIndex = 0;
			this->ptbSrc->TabStop = false;
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->textBox1);
			this->groupBox2->Controls->Add(this->label3);
			this->groupBox2->Controls->Add(this->label2);
			this->groupBox2->Controls->Add(this->label1);
			this->groupBox2->Controls->Add(this->ptbCha);
			this->groupBox2->Controls->Add(this->ptbPlate);
			this->groupBox2->Location = System::Drawing::Point(640, 12);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(158, 224);
			this->groupBox2->TabIndex = 0;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Result";
			// 
			// textBox1
			// 
			this->textBox1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->textBox1->Location = System::Drawing::Point(6, 172);
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(146, 31);
			this->textBox1->TabIndex = 2;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(7, 156);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(80, 13);
			this->label3->TabIndex = 1;
			this->label3->Text = L"Charater Result";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(7, 86);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(47, 13);
			this->label2->TabIndex = 1;
			this->label2->Text = L"Charater";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(7, 20);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(31, 13);
			this->label1->TabIndex = 1;
			this->label1->Text = L"Plate";
			// 
			// ptbCha
			// 
			this->ptbCha->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->ptbCha->Location = System::Drawing::Point(6, 102);
			this->ptbCha->Name = L"ptbCha";
			this->ptbCha->Size = System::Drawing::Size(146, 46);
			this->ptbCha->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->ptbCha->TabIndex = 0;
			this->ptbCha->TabStop = false;
			// 
			// ptbPlate
			// 
			this->ptbPlate->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->ptbPlate->Location = System::Drawing::Point(6, 36);
			this->ptbPlate->Name = L"ptbPlate";
			this->ptbPlate->Size = System::Drawing::Size(146, 46);
			this->ptbPlate->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->ptbPlate->TabIndex = 0;
			this->ptbPlate->TabStop = false;
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->btnLearn);
			this->groupBox3->Controls->Add(this->btnProcess);
			this->groupBox3->Controls->Add(this->btnLoad);
			this->groupBox3->Location = System::Drawing::Point(640, 376);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(158, 110);
			this->groupBox3->TabIndex = 1;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"Control Bar";
			// 
			// btnLearn
			// 
			this->btnLearn->Location = System::Drawing::Point(10, 77);
			this->btnLearn->Name = L"btnLearn";
			this->btnLearn->Size = System::Drawing::Size(142, 23);
			this->btnLearn->TabIndex = 0;
			this->btnLearn->Text = L"Machine Learning";
			this->btnLearn->UseVisualStyleBackColor = true;
			this->btnLearn->Click += gcnew System::EventHandler(this, &MainForm::btnLearn_Click);
			// 
			// btnProcess
			// 
			this->btnProcess->Location = System::Drawing::Point(10, 48);
			this->btnProcess->Name = L"btnProcess";
			this->btnProcess->Size = System::Drawing::Size(142, 23);
			this->btnProcess->TabIndex = 0;
			this->btnProcess->Text = L"Processing";
			this->btnProcess->UseVisualStyleBackColor = true;
			this->btnProcess->Click += gcnew System::EventHandler(this, &MainForm::btnProcess_Click);
			// 
			// btnLoad
			// 
			this->btnLoad->Location = System::Drawing::Point(10, 19);
			this->btnLoad->Name = L"btnLoad";
			this->btnLoad->Size = System::Drawing::Size(142, 23);
			this->btnLoad->TabIndex = 0;
			this->btnLoad->Text = L"Load Image";
			this->btnLoad->UseVisualStyleBackColor = true;
			this->btnLoad->Click += gcnew System::EventHandler(this, &MainForm::btnLoad_Click);
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(643, 253);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(78, 13);
			this->label4->TabIndex = 2;
			this->label4->Text = L"SrcImage Size:";
			// 
			// imgSize
			// 
			this->imgSize->AutoSize = true;
			this->imgSize->Location = System::Drawing::Point(727, 253);
			this->imgSize->Name = L"imgSize";
			this->imgSize->Size = System::Drawing::Size(0, 13);
			this->imgSize->TabIndex = 2;
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(810, 498);
			this->Controls->Add(this->imgSize);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->groupBox3);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->Name = L"MainForm";
			this->Text = L"MainForm";
			this->groupBox1->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ptbSrc))->EndInit();
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ptbCha))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ptbPlate))->EndInit();
			this->groupBox3->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion


	private: System::Void btnLoad_Click(System::Object^  sender, System::EventArgs^  e) {

		OpenFileDialog^ dgOpen = gcnew OpenFileDialog();
		dgOpen->Filter = "Image(*.bmp; *.jpg)|*.bmp;*.jpg|All files (*.*)|*.*||";
		if (dgOpen->ShowDialog() == System::Windows::Forms::DialogResult::Cancel)
		{
			return;
		}
		Bitmap^ bmpSrc = gcnew Bitmap(dgOpen->FileName);
		ptbSrc->Image = bmpSrc;
		ptbSrc->Refresh();
		srcImg = imread(stringTochar.ConvertString2Char(dgOpen->FileName));
		int dbSize_H = srcImg.rows;
		int dbSize_W = srcImg.cols;
		System::String^ h = gcnew System::String(to_string(dbSize_H).c_str());
		System::String^ w = gcnew System::String(to_string(dbSize_W).c_str());

		imgSize->Text = w + "x" + h;
		

	}
	private: System::Void btnLearn_Click(System::Object^  sender, System::EventArgs^  e) {

		FolderBrowserDialog^ folderOpen = gcnew FolderBrowserDialog();
		if (folderOpen->ShowDialog() == System::Windows::Forms::DialogResult::Cancel)
		{
			return;
		}

		System::String^ sysImgpath;
		sysImgpath = folderOpen->SelectedPath;
		char cStr[50] = { 0 };
		if (sysImgpath->Length < sizeof(cStr))
			sprintf(cStr, "%s", sysImgpath);
		std::string imgpath(cStr);

		SaveFileDialog^ saveFile= gcnew SaveFileDialog();
		if (saveFile->ShowDialog() == System::Windows::Forms::DialogResult::Cancel)
		{
			return;
		}
		System::String^ sysSaveimg = saveFile->FileName;
		char cStr_[50] = { 0 };
		if (sysSaveimg->Length < sizeof(cStr))
			sprintf(cStr_, "%s", sysSaveimg);
		std::string savesvm(cStr_);

		if (TrainSVM(savesvm,imgpath))
		{
			MessageBox::Show("Training completed.");
		}
		else
			MessageBox::Show("ERROR");
	}
	private: System::Void btnProcess_Click(System::Object^  sender, System::EventArgs^  e) {
		textBox1->Text = "";
		if (SVMPredict())
		{
			//do something
			//MessageBox::Show("Processing completed.");
		}
		else
			MessageBox::Show("ERROR");
	}


	private: vector<string> list_folder(string path)
	{
		vector<string> folders;
		DIR *dir = opendir(path.c_str());
		struct dirent *entry;
		while ((entry = readdir(dir)) != NULL)
		{
			if ((strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0))
			{
				string folder_path = path + "/" + string(entry->d_name);
				folders.push_back(folder_path);
			}
		}
		closedir(dir);
		return folders;

	}
	private: vector<string> list_file(string folder_path)
	{
		vector<string> files;
		DIR *dir = opendir(folder_path.c_str());
		struct dirent *entry;
		while ((entry = readdir(dir)) != NULL)
		{
			if ((strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0))
			{
				string file_path = folder_path + "/" + string(entry->d_name);
				files.push_back(file_path);
			}
		}
		closedir(dir);
		return files;
	}
	private: char character_recognition(Mat img_character)
	{
		//Load SVM training file OpenCV 3.1
		Ptr<SVM> svmNew = SVM::create();
		svmNew = SVM::load<SVM>("svm.txt");

		//SVM svmNew;
		//svmNew.load("D:/svm.txt");

		char c = '*';

		vector<float> feature = calculate_feature(img_character);
		// Open CV3.1
		Mat m = Mat(1, number_of_feature, CV_32FC1);  
		for (size_t i = 0; i < feature.size(); ++i)
		{
			float temp = feature[i];
			m.at<float>(0, i) = temp;
		}
		//Mat m = Mat(number_of_feature,1, CV_32FC1);		// Open CV 2.4
		//for (size_t i = 0; i < feature.size(); ++i)
		//{
		//float temp = feature[i];
		//m.at<float>(i,0) = temp;
		//}
		int ri = int(svmNew->predict(m)); // Open CV 3.1
		/*int ri = int(svmNew.predict(m));*/
		if (ri >= 0 && ri <= 9)
			c = (char)(ri + 48); //ma ascii 0 = 48
		if (ri >= 10 && ri < 18)
			c = (char)(ri + 55); //ma accii A = 5, --> tu A-H
		if (ri >= 18 && ri < 22)
			c = (char)(ri + 55 + 2); //K-N, bo I,J
		if (ri == 22) c = 'P';
		if (ri == 23) c = 'S';
		if (ri >= 24 && ri < 27)
			c = (char)(ri + 60); //T-V,  
		if (ri >= 27 && ri < 30)
			c = (char)(ri + 61); //X-Z

		return c;

	}
	private: bool TrainSVM(string savepath, string trainImgpath) {
		const int number_of_class = 30;
		const int number_of_sample = 10;
		const int number_of_feature = 32;

		//Train SVM OpenCV 3.1
		Ptr<SVM> svm = SVM::create();
		svm->setType(SVM::C_SVC);
		svm->setKernel(SVM::RBF);
		svm->setGamma(0.5);
		svm->setC(16);
		svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));

		vector<string> folders = list_folder(trainImgpath);
		if (folders.size() <= 0)
		{
			//do something
			return false;
		}
		if (number_of_class != folders.size() || number_of_sample <= 0 || number_of_class <= 0)
		{
			//do something
			return false;
		}
		Mat src;
		Mat data = Mat(number_of_sample * number_of_class, number_of_feature, CV_32FC1);
		Mat label = Mat(number_of_sample * number_of_class, 1, CV_32SC1);
		int index = 0;
		for (size_t i = 0; i < folders.size(); ++i)
		{
			vector<string> files = list_file(folders.at(i));
			if (files.size() <= 0 || files.size() != number_of_sample)
			{
				return false;
			}
			string folder_path = folders.at(i);
			string label_folder = folder_path.substr(folder_path.length() - 1);
			for (size_t j = 0; j < files.size(); ++j)
			{
				src = imread(files.at(j));
				if (src.empty())
				{
					return false;
				}
				vector<float> feature = calculate_feature(src);
				for (size_t t = 0; t < feature.size(); ++t)
					data.at<float>(index, t) = feature.at(t);
				label.at<int>(index, 0) = i;
				index++;
			}
		}
		// SVM Train OpenCV 3.1
		svm->trainAuto(ml::TrainData::create(data, ml::ROW_SAMPLE, label));
		svm->save(savepath);
		return true;
	}
	private: bool SVMPredict() {
		vector<Mat> plates;
		vector<Mat> draw_character;
		vector<vector<Mat> > characters;
		vector<string> text_recognition;
		vector<double> process_time;
		void sort_character(vector<Mat>&);
		Mat image = srcImg;
		Mat gray, binary;
		vector<vector<cv::Point> > contours;
		vector<Vec4i> hierarchy;
		double t = (double)cvGetTickCount();
		cvtColor(image, gray, CV_BGR2GRAY);
		//imshow("gray", gray);
		adaptiveThreshold(gray, binary, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 55, 5);
		//imshow("binary",binary);
		Mat or_binary = binary.clone();
		Mat element = getStructuringElement(MORPH_RECT, cv::Size(3,3));
		erode(binary, binary, element);
		dilate(binary, binary, element);
		findContours(binary, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
		if (contours.size() <= 0) return false;
		for (size_t i = 0; i < contours.size(); ++i)
		{
			Rect r = boundingRect(contours.at(i));
			if (r.width > image.cols / 2 || r.height > image.cols / 2 || r.width < 120 || r.height < 20
				|| (double)r.width / r.height > 4.5 || (double)r.width / r.height < 3.5)
				continue;
			Mat sub_binary = or_binary(r);
			Mat _plate = sub_binary.clone();
			vector<vector<cv::Point> > sub_contours;
			vector<Vec4i> sub_hierarchy;
			findContours(sub_binary, sub_contours, sub_hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
			if (sub_contours.size() < 8) continue;
			int count = 0;
			vector<Mat> c;
			Mat sub_image = image(r);
			vector<Rect> r_characters;
			for (size_t j = 0; j < sub_contours.size(); ++j)
			{
				Rect sub_r = boundingRect(sub_contours.at(j));
				if (sub_r.height > r.height / 2 && sub_r.width < r.width / 8 && sub_r.width > 5 && r.width > 15 && sub_r.x > 5)
				{
					Mat cj = _plate(sub_r);
					double ratio = (double)count_pixel(cj) / (cj.cols*cj.rows);
					if (ratio > 0.2 && ratio < 0.7)
					{
						r_characters.push_back(sub_r);
						rectangle(sub_image, sub_r, Scalar(0, 0, 255), 2, 8, 0);
					}
				}
			}
			if (r_characters.size() >= 7)
			{
				// sap xep 
				for (int i = 0; i < r_characters.size() - 1; ++i)
				{
					for (int j = i + 1; j < r_characters.size(); ++j)
					{
						Rect temp;
						if (r_characters.at(j).x < r_characters.at(i).x)
						{
							temp = r_characters.at(j);
							r_characters.at(j) = r_characters.at(i);
							r_characters.at(i) = temp;
						}
					}
				}
				for (int i = 0; i < r_characters.size(); ++i)
				{
					Mat cj = _plate(r_characters.at(i));
					c.push_back(cj);
				}
				characters.push_back(c);
				sub_binary = or_binary(r);
				plates.push_back(_plate);
				draw_character.push_back(sub_image);
			}
			rectangle(image, r, Scalar(0, 255, 0), 2, 8, 0);
		}

		//imshow("plate", image);
		//imshow("char", draw_character[0]);
		Mat plateImg, chaImg;
		convertScaleAbs(plates[0], plateImg);
		convertScaleAbs(draw_character[0], chaImg);
		resize(plateImg, plateImg, cv::Size(200, 50));
		resize(chaImg, chaImg, cv::Size(200, 50));
		ptbPlate->Image = mat2bmp.Mat2Bimap(plateImg);
		ptbCha->Image = mat2bmp.Mat2Bimap(chaImg);
		ptbPlate->Refresh();
		ptbCha->Refresh();

		// Plate recoginatinon
		for (size_t i = 0; i < characters.size(); i++)
		{
			string result;
			for (size_t j = 0; j < characters.at(i).size(); ++j)
			{

				char cs = character_recognition(characters.at(i).at(j));
				result.push_back(cs);

			}
			text_recognition.push_back(result);
			System::String^ str = gcnew System::String(result.c_str()); // Convert std string to System String
			textBox1->Text += str;
		}
		return true;
	}
};
}
