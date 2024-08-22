#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/modify/FLAlertLayer.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>

bool blockKeys = false;

class $modify(MyHookLol, CCKeyboardDispatcher) {
	// Needed because BetterInfo has special FLAlertLayers that duplicate for some reason
	bool dispatchKeyboardMSG(enumKeyCodes key, bool down, bool idk) {
		if (blockKeys && down && Loader::get()->isModLoaded("cvolton.betterinfo")) {
			if (key == enumKeyCodes::KEY_Left || key == enumKeyCodes::KEY_Right)
				return true;
			else if (key == enumKeyCodes::KEY_Escape)
				blockKeys = false;
		}

		return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, idk);
	}
};
class $modify(MyFLAlertLayer, FLAlertLayer) {
	struct Fields {
		int linesProgressed = 0;
		bool done = false;
		gd::string text = "";
		float screenSize = CCDirector::sharedDirector()->getWinSize().width;
		int textSize = 36;
		int btnSelected = 0;
		bool dontRestrictWidth = Mod::get()->getSettingValue<bool>("dontRestrictWidth");
		bool disableClickToProgress = Mod::get()->getSettingValue<bool>("disableClickToProgress");
		CCNode* mainLayer;
		CCNode* btn1;
		CCNode* btn2;
		CCNode* textAreaClippingNode;
		CCNode* textArea;
		CCNode* gradientOverlay;
		CCNode* bg;
		CCNode* title;
	};
	bool init(FLAlertLayerProtocol * delegate, char const* title, gd::string desc, char const* btn1, char const* btn2, float width, bool scroll, float height, float textScale) {
		if (m_fields->screenSize >= 569 && !m_fields->dontRestrictWidth)
			m_fields->screenSize = 569;

		width = m_fields->screenSize;
		m_fields->text = desc;
		height = 140;
		scroll = false;
		textScale = 1;
		this->m_noElasticity = true;
		bool ret = FLAlertLayer::init(delegate, title, desc, btn1, btn2, width, scroll, height, textScale);

		NodeIDs::provideFor(this);

		if (m_fields->mainLayer = this->getChildByID("main-layer")) {
			if (this->m_buttonMenu) {
				m_fields->btn1 = this->m_buttonMenu->getChildByID("button-1");
				m_fields->btn2 = this->m_buttonMenu->getChildByID("button-2");
			}
			m_fields->textAreaClippingNode = m_fields->mainLayer->getChildByID("content-text-area");
			m_fields->bg = m_fields->mainLayer->getChildByID("background");
			m_fields->title = m_fields->mainLayer->getChildByID("title");
		}
		this->setID("FLAlertLayer");
		return ret;
	}
	void changeBG() {
		m_fields->bg->removeFromParent();

		m_fields->bg = CCScale9Sprite::create("deltaruneSquare.png"_spr);
		m_fields->bg->setContentHeight(140);
		m_fields->bg->setContentWidth(m_fields->screenSize);
		m_fields->bg->setPosition(CCPoint{ CCDirector::sharedDirector()->getWinSize().width / 2, 70 });
		m_fields->bg->setID("background");

		m_fields->mainLayer->addChild(m_fields->bg);
	}
	void changeButtons() {
		if (!this->m_buttonMenu) return;
		this->m_buttonMenu->setPositionY(32);
		this->m_buttonMenu->setVisible(false);

		if (!m_fields->btn2) return;

		m_fields->btn1->setPositionX(m_fields->bg->getPositionX() - this->m_buttonMenu->getPositionX() - m_fields->screenSize / 2 + m_fields->screenSize / 4);
		m_fields->btn2->setPositionX(m_fields->bg->getPositionX() - this->m_buttonMenu->getPositionX() - m_fields->screenSize / 2 + (m_fields->screenSize / 4) * 3);

		CCArrayExt<CCNode*> buttons = this->m_buttonMenu->getChildren();

		auto heart = CCSprite::create("heart.png"_spr);
		heart->setPositionX(-m_fields->btn1->getPositionX() + static_cast<CCNode*>(m_fields->btn1->getChildren()->objectAtIndex(0))->getPositionX());
		heart->setPositionY(m_fields->btn1->getPositionY() + m_fields->btn1->getContentHeight() / 2);
		heart->setID("heart");
		static_cast<CCNode*>(m_fields->btn1->getChildren()->objectAtIndex(0))->addChild(heart);

		for (auto button : buttons) {
			if (button != typeinfo_cast<CCMenuItemSpriteExtra*>(button)) continue;
			static_cast<CCMenuItemSpriteExtra*>(button)->m_animationEnabled = false;
			CCArrayExt<CCNode*> parts = static_cast<CCNode*>(button->getChildren()->objectAtIndex(0))->getChildren();
			for (auto part : parts) {
				if (auto buttonTexture = typeinfo_cast<CCScale9Sprite*>(part)) {
					buttonTexture->setVisible(false);
				}
				else if (auto label = typeinfo_cast<CCLabelBMFont*>(part)) {
					label->setFntFile("Determination.fnt"_spr);
					label->setScale(1);
				}
			}
		}
	}
	void changeTitle() {
		m_fields->title->setAnchorPoint(CCPoint{ 0, 0.5 });
		static_cast<CCLabelBMFont*>(m_fields->title)->setFntFile("Determination.fnt"_spr);
		m_fields->title->setPosition(CCPoint{ m_fields->bg->getPositionX() - m_fields->bg->getContentWidth() / 2 + 24, 145 });
	}
	void changeText() {
		m_fields->textAreaClippingNode->removeFromParent();
		auto newDesc = TextArea::create(
			m_fields->text,
			"Determination.fnt"_spr,
			1,
			m_fields->screenSize - 100,
			CCPoint{ 0, 1 },
			m_fields->textSize,
			false
		);
		newDesc->setContentWidth(m_fields->screenSize);
		newDesc->setAnchorPoint(CCPoint{ 0, 1 });
		newDesc->setPositionX(m_fields->bg->getPositionX() - m_fields->screenSize / 2 + 24);
		newDesc->setPositionY(110);
		newDesc->setZOrder(m_fields->textAreaClippingNode->getZOrder());
		newDesc->setID("content-text-area");

		auto newDescGrad = TextArea::create(
			m_fields->text,
			"DeterminationGradient.fnt"_spr,
			1,
			m_fields->screenSize - 100,
			CCPoint{ 0, 1 },
			m_fields->textSize,
			false
		);
		newDescGrad->setContentWidth(m_fields->screenSize);
		newDescGrad->setAnchorPoint(CCPoint{ 0, 1 });
		newDescGrad->setPositionX(m_fields->bg->getPositionX() - m_fields->screenSize / 2 + 24);
		newDescGrad->setPositionY(110);
		newDescGrad->setZOrder(m_fields->textAreaClippingNode->getZOrder() + 1);
		newDescGrad->setID("gradient-overlay"_spr);

		CCArrayExt<CCLabelBMFont*> lines = static_cast<CCNode*>(newDescGrad->getChildren()->objectAtIndex(0))->getChildren();

		for (auto line : lines) {
			CCArrayExt<CCSprite*> letters = line->getChildren();
			for (auto letter : letters) {
				letter->setColor(ccColor3B{ 255, 255, 255 });
			}
		}

		auto clippingNode = CCClippingNode::create(CCLayerColor::create({ 0,0,0,0 }, m_fields->bg->getContentWidth(), m_fields->bg->getContentHeight() - 20));
		clippingNode->setID("content-text-area"_spr);
		clippingNode->setPositionY(10);
		clippingNode->addChild(newDesc);
		clippingNode->addChild(newDescGrad);
		m_fields->mainLayer->addChild(clippingNode);
		m_fields->textAreaClippingNode = clippingNode;
		m_fields->textArea = newDesc;
		m_fields->gradientOverlay = newDescGrad;
	}
	void showButtons(CCArrayExt<CCLabelBMFont*> content) {
		if (m_fields->btn2 && content.size() < 3) {
			m_fields->done = true;
			this->m_buttonMenu->setVisible(true);
		}
	}
	void changeLook() {
		if (!m_fields->bg) return;
		if (!m_fields->title) return;
		if (!m_fields->textAreaClippingNode) return;
		if (!m_fields->mainLayer) return;

		changeBG();
		changeButtons();
		changeTitle();
		changeText();
		CCArrayExt<CCLabelBMFont*> content = static_cast<CCNode*>(m_fields->textAreaClippingNode->getChildren()->objectAtIndex(0))->getChildren();
		showButtons(content);
	}
	// I can't check for enter key so I guess I have to hook these
	void onBtn2(CCObject * sender) {
		if (!m_fields->done) {
			progressText();
			return;
		}
		if (m_fields->btnSelected == 0) return;
		if (m_fields->btnSelected == 2)
			FLAlertLayer::onBtn2(sender);
		else if (m_fields->btnSelected == 1)
			FLAlertLayer::onBtn1(sender);
	}
	void onBtn1(CCObject * sender) {
		if (!m_fields->done) {
			progressText();
			return;
		}
		blockKeys = false;
		FLAlertLayer::onBtn1(sender);
	}
	int getLinesLeft(auto & content) {
		return content.size() - m_fields->linesProgressed;
	}
	void progressText() {
		if (!m_fields->mainLayer) return;
		if (!this->m_buttonMenu) return;
		if (!m_fields->textAreaClippingNode) return;
		if (!m_fields->textArea) return;

		auto mlbmFont = m_fields->textArea->getChildren()->objectAtIndex(0);
		auto fontNode = static_cast<CCNode*>(mlbmFont);
		CCArrayExt<CCLabelBMFont*> content = fontNode->getChildren();

		if (getLinesLeft(content) <= 3) {
			if (!m_fields->btn2) {
				m_fields->done = true;
				FLAlertLayer::onBtn1(m_fields->btn1);
				return;
			}
			else if (m_fields->btnSelected != 0) {
				m_fields->done = true;
				if (m_fields->btnSelected == 1)
					FLAlertLayer::onBtn1(m_fields->btn1);
				else if (m_fields->btnSelected == 2)
					FLAlertLayer::onBtn2(m_fields->btn2);
				return;
			}
		}

		// move EVERYTHING up
		int i = 0;
		int offset;

		if (getLinesLeft(content) > 3) 
			offset = 3;
		else if (getLinesLeft(content) == 3) 
			offset = 2;
		m_fields->linesProgressed += offset;
		m_fields->textArea->setPositionY(m_fields->textArea->getPositionY() + m_fields->textSize * offset);
		m_fields->gradientOverlay->setPositionY(m_fields->textArea->getPositionY());

		if (m_fields->btn2 && getLinesLeft(content) < 3) {
			m_fields->done = true;
			this->m_buttonMenu->setVisible(true);
		}
	}
	void show() {
		FLAlertLayer::show();
		int numOfSiblings = 0;
		if (auto parent = this->getParent()) {
			CCArrayExt<CCNode*> siblings = parent->getChildren();

			for (auto sibling : siblings) {
				if (sibling->getID() == "FLAlertLayer")
					numOfSiblings++;
			}
		}
		if (numOfSiblings >= 1 && !m_fields->btn2)
			blockKeys = true;
		else
			blockKeys = false;

		changeLook();
	}
	void addHeart(CCNode * parent, CCLabelBMFont * label) {
		if (parent->getChildByID("heart")) {
			parent->getChildByID("heart")->removeFromParentAndCleanup(true);
		}
		auto heart = CCSprite::create("heart.png"_spr);
		heart->setAnchorPoint(CCPoint{ 1, 0.5 });
		heart->setPosition(CCPoint{ label->getPositionX() - 5 - label->getContentWidth() / 2, label->getPositionY() - 2 });
		heart->setID("heart");
		parent->addChild(heart);
	}
	bool ccTouchBegan(CCTouch * touch, CCEvent * event) {
		if (!m_fields->done && !m_fields->disableClickToProgress)
			progressText();
		bool ret = FLAlertLayer::ccTouchBegan(touch, event);
		if (!m_fields->mainLayer) return ret;
		if (!this->m_buttonMenu) return ret;
		CCArrayExt<CCMenuItemSpriteExtra*> buttons = this->m_buttonMenu->getChildren();
		bool selected = false;
		for (auto button : buttons) {
			if (button->isSelected()) {
				selected = true;
			}
		}
		for (auto button : buttons) {
			auto parent = static_cast<CCNode*>(button->getChildren()->objectAtIndex(0));
			CCArrayExt<CCNode*> parts = parent->getChildren();
			for (auto part : parts) {
				if (auto label = typeinfo_cast<CCLabelBMFont*>(part)) {
					if (button->isSelected()) {
						label->setColor(ccColor3B{ 255, 255, 0 });
						addHeart(parent, label);
						if (button->getID() == "button-1")
							m_fields->btnSelected = 1;
						else if (button->getID() == "button-2")
							m_fields->btnSelected = 2;
					}
					else if (selected) {
						label->setColor(ccColor3B{ 255,255,255 });
						auto heart = parent->getChildByID("heart");
						if (heart)
							heart->removeFromParentAndCleanup(true);
					}
				}
			}
		}
		return ret;
	}
	void keyDown(enumKeyCodes key) {
		if (key == enumKeyCodes::KEY_Z || key == enumKeyCodes::CONTROLLER_A) {
			progressText();
			return;
		}
		else if (key == enumKeyCodes::KEY_ArrowLeft || key == enumKeyCodes::KEY_ArrowRight || key == enumKeyCodes::KEY_Left || key == enumKeyCodes::KEY_Right) {
			if (!m_fields->mainLayer) {
				FLAlertLayer::keyDown(key);
				return;
			}
			if (!m_fields->btn2) {
				FLAlertLayer::keyDown(key);
				return;
			}

			auto parent = static_cast<CCNode*>(m_fields->btn1->getChildren()->objectAtIndex(0));
			CCArrayExt<CCNode*> partsBtn1 = parent->getChildren();
			for (auto part : partsBtn1) {
				if (auto label = typeinfo_cast<CCLabelBMFont*>(part)) {
					if (key == enumKeyCodes::KEY_ArrowLeft || key == enumKeyCodes::KEY_Left) {
						m_fields->btnSelected = 1;
						addHeart(parent, label);
						label->setColor(ccColor3B{ 255,255,0 });
					}
					else if (key == enumKeyCodes::KEY_ArrowRight || key == enumKeyCodes::KEY_Right) {
						label->setColor(ccColor3B{ 255,255,255 });
						auto heart = parent->getChildByID("heart");
						if (heart)
							heart->removeFromParentAndCleanup(true);
					}
				}
			}
			parent = static_cast<CCNode*>(m_fields->btn2->getChildren()->objectAtIndex(0));
			CCArrayExt<CCNode*> partsBtn2 = parent->getChildren();
			for (auto part : partsBtn2) {
				if (auto label = typeinfo_cast<CCLabelBMFont*>(part)) {
					if (key == enumKeyCodes::KEY_ArrowLeft || key == enumKeyCodes::KEY_Left) {
						label->setColor(ccColor3B{ 255,255,255 });
						auto heart = parent->getChildByID("heart");
						if (heart)
							heart->removeFromParentAndCleanup(true);
					}
					else if (key == enumKeyCodes::KEY_ArrowRight || key == enumKeyCodes::KEY_Right) {
						m_fields->btnSelected = 2;
						addHeart(parent, label);
						label->setColor(ccColor3B{ 255,255,0 });
					}
				}
			}
		}
		else FLAlertLayer::keyDown(key);
	}
};