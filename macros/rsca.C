// read scaler
{
	e->ClearCnaf();
	e->SetNofCnafs(10);
	e->SetCnaf(0, crate1, station1, 2, 0);
	e->SetCnaf(1, crate1, station1, 3, 0);
	nCnafs = 2;
	if (s2) {
		e->SetCnaf(2, crate2, station2, 2, 0);
		e->SetCnaf(3, crate2, station2, 3, 0);
		nCnafs = 4;
	}
	if (s3) {
		e->SetCnaf(4, crate2, station2, 2, 0);
		e->SetCnaf(5, crate2, station2, 3, 0);
		nCnafs = 6;
	}
	e->ClearData();
	e->ExecCnafList(nCnafs);
	e->Print();

	msb = e->GetData(0);
	lsb = e->GetData(1);
	data1 = (((msb & 0xffffL) << 16) & 0xffff0000L) | (lsb & 0xffffL);
	cout << "rsca: Scaler " << s1->GetName() << " in " << s1->GetPosition() << ": Data = " << data1 << endl;
	if (s2) {
		msb = e->GetData(2);
		lsb = e->GetData(3);
		data2 = (((msb & 0xffffL) << 16) & 0xffff0000L) | (lsb & 0xffffL);
		cout << "rsca: Scaler " << s2->GetName() << " in " << s2->GetPosition() << ": Data = " << data2 << endl;
	}
	if (s3) {
		msb = e->GetData(4);
		lsb = e->GetData(5);
		data3 = (((msb & 0xffffL) << 16) & 0xffff0000L) | (lsb & 0xffffL);
		cout << "rsca: Scaler " << s2->GetName() << " in " << s2->GetPosition() << ": Data = " << data3 << endl;
	}
}
