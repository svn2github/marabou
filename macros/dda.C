{
	gROOT->Macro("LoadLibs-Test.C");
	cout << "[Loading Snake lib]" << endl;
	gSystem->Load("/home/rg/marabou/src/snake/libTSnkDDA0816.so");
	dda = new TSnkDDA0816();

	dda->SetOffset(0, 0);
	dda->SetRange(0, 1000);
	dda->SetIncrement(0, 2);
	dda->SetWaveForm(0, TSnkDDAChannel::kWFTriangle);

	dda->SetPreScaler(1, TSnkDDAChannel::kPS100k);
	dda->SetOffset(1, 0);
	dda->SetRange(1, 2000);
	dda->SetIncrement(1, 1);
	dda->SetWaveForm(1, TSnkDDAChannel::kWFTriangle);

	dda->SetCurveAsymm();
	dda->Print();
}
