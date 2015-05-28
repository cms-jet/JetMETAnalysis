void run() {
   gROOT->LoadMacro("responseByHand.C");

   compareErasNoPU("ak4pf","resp",10,20);
   compareErasNoPU("ak4pf","mass",10,20);
   compareErasNoPU("ak4pf","respCrossCheck",10,20);
   compareErasNoPU("ak4pf","CHF",10,20);
   compareErasNoPU("ak4pf","NHF",10,20);
   compareErasNoPU("ak4pf","NEF",10,20);
   compareErasNoPU("ak4pf","resp",20,40);
   compareErasNoPU("ak4pf","mass",20,40);
   compareErasNoPU("ak4pf","respCrossCheck",20,40);
   compareErasNoPU("ak4pf","CHF",20,40);
   compareErasNoPU("ak4pf","NHF",20,40);
   compareErasNoPU("ak4pf","NEF",20,40);
   compareErasNoPU("ak4pf","resp",90,110);
   compareErasNoPU("ak4pf","mass",90,110);
   compareErasNoPU("ak4pf","respCrossCheck",90,110);
   compareErasNoPU("ak4pf","CHF",90,110);
   compareErasNoPU("ak4pf","NHF",90,110);
   compareErasNoPU("ak4pf","NEF",90,110);
   compareErasNoPU("ak4pf","resp",200,300);
   compareErasNoPU("ak4pf","mass",200,300);
   compareErasNoPU("ak4pf","respCrossCheck",200,300);
   compareErasNoPU("ak4pf","CHF",200,300);
   compareErasNoPU("ak4pf","NHF",200,300);
   compareErasNoPU("ak4pf","NEF",200,300);
}
